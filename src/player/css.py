import re
import json
import httpx
from lxml import html
from pathlib import Path
from urllib.parse import urljoin
from difflib import SequenceMatcher
from playwright.sync_api import sync_playwright


class VideoCrawler:
    def __init__(self, config_file="css.json"):
        """初始化爬虫"""
        config_path = Path(__file__).parent / config_file
        self.config = json.loads(config_path.read_text(encoding='utf-8'))
        self.site_configs = self.config.get("site_configs", {})

    def search_site(self, keyword, site_id):
        site_config = self.site_configs.get(site_id)
        url = site_config["base_url"] + site_config["search_path"].replace("{keyword}", keyword)
        headers = {
            'User-Agent': site_config.get("user_agent"),
            'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
            'Accept-Language': 'zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2',
            'Accept-Encoding': 'gzip, deflate, br',
            'Connection': 'keep-alive',
            'Upgrade-Insecure-Requests': '1',
        }
        try:
            response = httpx.get(url, headers=headers, timeout=10)
            response.encoding = 'utf-8'
            tree = html.fromstring(response.text)
            titles = tree.cssselect(site_config["title"])
            links = tree.cssselect(site_config["link"])
            results = []
            for title_elem, link_elem in zip(titles, links):
                title = title_elem.text_content().strip()
                link = urljoin(site_config["base_url"], link_elem.get('href'))
                similarity = SequenceMatcher(None, keyword, title).ratio()
                if link.startswith("http://"):
                    link = link.replace("http://", "https://", 1)
                results.append({'title': title, 'link': link, 'similarity': similarity})
            if results:
                best_result = max(results, key=lambda x: x['similarity'])
                routes = self.get_routes(best_result['link'], site_id)
                return {'title': best_result['title'], 'link': best_result['link'], 'similarity': best_result['similarity'], 'site': site_id, 'routes': routes}
        except Exception as e:
            print(f"站点 {site_id} 搜索失败: {e}")
        return None

    def get_routes(self, page_url, site_id):
        site_config = self.site_configs.get(site_id)
        headers = {'User-Agent': site_config.get("user_agent")}
        try:
            response = httpx.get(page_url, headers=headers, timeout=10)
            response.encoding = 'utf-8'
            tree = html.fromstring(response.text)
            routes = []
            route_tabs = tree.cssselect(site_config["route_tabs"])
            episode_containers = tree.cssselect(site_config["episode_containers"])
            for route_tab, container in zip(route_tabs, episode_containers):
                route_name = route_tab.text_content().strip()
                episodes = []
                for ep in container.cssselect(site_config["episode_items"]):
                    episode_name = ep.text_content().strip()
                    episode_link = ep.get('href')
                    episodes.append({'name': episode_name, 'link': urljoin(page_url, episode_link)})
                routes.append({'route': route_name, 'episodes': episodes})
            return routes
        except Exception as e:
            print(f"获取线路失败: {e}")
            return []

    def find_video_stream(self, episode_url, site_id):
        """捕获视频播放链接"""
        site_config = self.site_configs.get(site_id)
        pattern = re.compile('|'.join(site_config["video_patterns"]), re.IGNORECASE)
        with sync_playwright() as p:
            browser = p.chromium.launch(headless=True)
            page = browser.new_page()
            video_url = None
            found = False
            def on_request(request):
                nonlocal video_url, found
                if not found and pattern.search(request.url):
                    video_url = request.url
                    found = True
            page.on("request", on_request)
            try:
                page.goto(episode_url)
                max_checks = 30
                for _ in range(max_checks):
                    if found:
                        break
                    page.wait_for_timeout(500)
                return video_url
            except Exception as e:
                print(f"访问页面时出错: {e}")
                return None
            finally:
                browser.close()