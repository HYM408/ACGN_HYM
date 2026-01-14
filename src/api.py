import time
import httpx
import threading
import urllib.parse
from urllib.parse import urlparse, parse_qs
from http.server import BaseHTTPRequestHandler, HTTPServer
from src.config import get_config_item, set_config_items


class BangumiAPI:
    def __init__(self):
        self.load_config()
        self.base_url = "https://api.bgm.tv"

    def load_config(self):
        """加载配置文件"""
        self.user_id = get_config_item("user_id")
        self.token = get_config_item("access_token")
        self.refresh_token = get_config_item("refresh_token")

    def refresh_and_reload(self):
        """刷新token"""
        BangumiOAuth().exchange_code_for_token(refresh_token=self.refresh_token)
        self.load_config()

    def _create_client(self):
        """创建httpx客户端"""
        headers = {"User-Agent": "ACGN_HYM/1.0", "Authorization": f"Bearer {self.token}"}
        return httpx.Client(http2=False, headers=headers, timeout=20)

    def get_user_collections(self, max_retries=3, get_all=True):
        """获取Bangumi收藏"""
        all_items, offset = [], 0
        with self._create_client() as client:
            while True:
                for retry in range(max_retries):
                    try:
                        response = client.get(f"{self.base_url}/v0/users/{self.user_id}/collections",params={"limit": 50, "offset": offset})
                        if response.status_code == 401:
                            self.refresh_and_reload()
                            continue
                        response.raise_for_status()
                        data = response.json()
                        items = data.get("data", [])
                        print(f"批次 {offset // 50 + 1}")
                        all_items.extend(items)
                        total = data.get("total", 0)
                        if not get_all or offset + 50 >= total:
                            return all_items
                        offset += 50
                        break
                    except Exception:
                        if retry < max_retries - 1:
                            time.sleep(10)
                        else:
                            return None

    def get_user_collection(self, subject_id, max_retries=3):
        """获取Bangumi单个收藏"""
        for retry in range(max_retries):
            try:
                with self._create_client() as client:
                    response = client.get(f"{self.base_url}/v0/users/{self.user_id}/collections/{subject_id}")
                    if response.status_code == 401:
                        self.refresh_and_reload()
                    response.raise_for_status()
                    return response.json()
            except Exception:
                if retry < max_retries - 1:
                    time.sleep(10)
        return None

    def get_subject_episodes(self, subject_id, max_retries=3):
        """获取Bangumi章节收藏"""
        for retry in range(max_retries):
            try:
                with self._create_client() as client:
                    all_items, offset = [], 0
                    while True:
                        params = {"limit": 1000, "offset": offset}
                        url = f"{self.base_url}/v0/users/-/collections/{subject_id}/episodes"
                        response = client.get(url, params=params)
                        if response.status_code == 401:
                            self.refresh_and_reload()
                        response.raise_for_status()
                        items = response.json().get("data", [])
                        if not items:
                            return all_items
                        all_items.extend(items)
                        if not items or len(items) < 1000:
                            return all_items
                        offset += 1000
            except Exception:
                if retry < max_retries - 1:
                    time.sleep(10)
        return None

    def search_subjects(self, keyword=None, tag=None, subject_type=2, max_retries=3):
        """Bangumi搜索"""
        for retry in range(max_retries):
            try:
                with self._create_client() as client:
                    if tag is not None:
                        params = {"sort": "heat", "filter": {"type": [subject_type], "tag": [tag]}}
                    else:
                        params = {"keyword": keyword, "sort": "heat", "filter": {"type": [subject_type]}}
                    response = client.post(f"{self.base_url}/v0/search/subjects?limit=20",json=params)
                    if response.status_code == 401:
                        self.refresh_and_reload()
                        continue
                    response.raise_for_status()
                    return response.json().get("data", [])
            except Exception:
                if retry < max_retries - 1:
                    time.sleep(10)
        return None

    def get_subject_info(self, subject_id, max_retries=3):
        """获取条目信息"""
        for retry in range(max_retries):
            try:
                with self._create_client() as client:
                    response = client.get(f"{self.base_url}/v0/subjects/{subject_id}")
                    if response.status_code == 401:
                        self.refresh_and_reload()
                        continue
                    response.raise_for_status()
                    return response.json()
            except Exception:
                if retry < max_retries - 1:
                    time.sleep(10)
        return None

    def create_or_update_collection(self, subject_id, collection_data, max_retries=3):
        """新增单个收藏条目"""
        for retry in range(max_retries):
            try:
                with self._create_client() as client:
                    response = client.post(f"{self.base_url}/v0/users/-/collections/{subject_id}",json=collection_data)
                    if response.status_code == 401:
                        self.refresh_and_reload()
                    response.raise_for_status()
                    return response
            except Exception:
                if retry < max_retries - 1:
                    time.sleep(10)
        return None

    def update_collection(self, subject_id, collection_data, max_retries=3):
        """修改单个收藏"""
        for retry in range(max_retries):
            try:
                with self._create_client() as client:
                    response = client.patch(f"{self.base_url}/v0/users/-/collections/{subject_id}",json=collection_data)
                    if response.status_code == 401:
                        self.refresh_and_reload()
                    response.raise_for_status()
                    return response
            except Exception:
                if retry < max_retries - 1:
                    time.sleep(10)
        return None

    def update_subject_episodes(self, subject_id, episodes_data, max_retries=3):
        """批量更新章节收藏信息"""
        for retry in range(max_retries):
            try:
                with self._create_client() as client:
                    response = client.patch(f"{self.base_url}/v0/users/-/collections/{subject_id}/episodes",json=episodes_data)
                    if response.status_code == 401:
                        self.refresh_and_reload()
                    response.raise_for_status()
                    return response
            except Exception:
                if retry < max_retries - 1:
                    time.sleep(10)
        return None


class BangumiOAuth:
    """Bangumi OAuth授权"""
    def __init__(self):
        self.client_id = get_config_item("client_id", "")
        self.client_secret = get_config_item("client_secret", "")
        self.redirect_uri = get_config_item("redirect_uri", "")
        self.client = httpx.Client(http2=False, timeout=30.0, headers={"User-Agent": "ACGN_HYM/1.0"})

    def __del__(self):
        """关闭客户端"""
        if hasattr(self, 'client'):
            self.client.close()

    def generate_auth_url(self):
        """获取Bangumi授权code"""
        base_url = "https://bgm.tv/oauth/authorize"
        params = {"client_id": self.client_id, "response_type": "code", "redirect_uri": self.redirect_uri}
        query_string = urllib.parse.urlencode(params)
        return f"{base_url}?{query_string}"

    def _create_callback_handler(self):
        """回调处理"""
        class CallbackHandler(BaseHTTPRequestHandler):
            """回调请求处理"""
            def do_GET(self):
                parsed_url = urlparse(self.path)
                query_params = parse_qs(parsed_url.query)
                code_list = query_params.get('code', [])
                self.send_response(200)
                self.send_header('Content-type', 'text/html; charset=utf-8')
                self.end_headers()
                if code_list:
                    self.server.code = code_list[0]
                    html_content = """<html><head><title>授权成功</title></head><body style="text-align: center; padding-top: 50px;"><p>授权成功，您可以关闭此窗口</p></body></html>"""
                else:
                    html_content = """<html><head><title>授权失败</title></head><body style="text-align: center; padding-top: 50px;"><p>授权失败，请重试</p></body></html>"""
                self.wfile.write(html_content.encode('utf-8'))
            def log_message(self, format, *args):
                pass
        return CallbackHandler

    def listen_for_code(self, timeout=60):
        """本地服务器监听回调，获取code"""
        parsed_uri = urlparse(self.redirect_uri)
        hostname = parsed_uri.hostname
        port = parsed_uri.port
        CallbackHandler = self._create_callback_handler()
        server = HTTPServer((hostname, port), CallbackHandler)
        server.code = None
        server_thread = threading.Thread(target=server.serve_forever)
        server_thread.daemon = True
        server_thread.start()
        try:
            start_time = time.time()
            while time.time() - start_time < timeout:
                if server.code is not None:
                    server.shutdown()
                    server_thread.join()
                    return server.code
                time.sleep(0.1)
            server.shutdown()
            server.server_close()
            return None
        except (KeyboardInterrupt, Exception):
            server.shutdown()
            server.server_close()
            return None

    def exchange_code_for_token(self, code=None, refresh_token=None):
        """换取token"""
        token_url = "https://bgm.tv/oauth/access_token"
        if refresh_token:
            data = {"grant_type": "refresh_token","client_id": self.client_id,"client_secret": self.client_secret,"refresh_token": refresh_token}
        else:
            data = {"grant_type": "authorization_code","client_id": self.client_id,"client_secret": self.client_secret,"code": code,"redirect_uri": self.redirect_uri}
        headers = {"Content-Type": "application/x-www-form-urlencoded", "User-Agent": "ACGN_HYM/1.0"}
        try:
            response = self.client.post(token_url, data=data, headers=headers)
            response.raise_for_status()
            token_data = response.json()
            expires_in = token_data.get("expires_in", 604800)
            expires_at = int(time.time()) + expires_in
            config_updates = {
                "access_token": token_data.get("access_token"),
                "refresh_token": token_data.get("refresh_token"),
                "expires_at": expires_at,
            }
            if user_id := token_data.get("user_id"):
                config_updates["user_id"] = user_id
            set_config_items(**config_updates)
            return token_data
        except Exception as e:
            print(f"失败: {e}")
            return None