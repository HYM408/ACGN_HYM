import os
import time
import httpx
import threading
from tqdm import tqdm
from typing import Optional, Dict
from concurrent.futures import ThreadPoolExecutor


class MultiThreadDownloader:
    def __init__(self, max_workers: int = 4, timeout: int = 30):
        """
        初始化下载器

        Args:
            max_workers: 最大线程数
            timeout: 请求超时时间（秒）
        """
        self.max_workers = max_workers
        self.timeout = timeout
        self.client = httpx.Client(timeout=timeout)
        self.progress_file = ".download_progress"

    def get_file_size(self, url: str) -> Optional[int]:
        """获取文件大小"""
        try:
            response = self.client.head(url)
            response.raise_for_status()
            return int(response.headers.get('content-length', 0))
        except Exception as e:
            print(f"获取文件大小失败: {e}")
            return None

    def download_chunk(
            self,
            url: str,
            start: int,
            end: int,
            output_file: str,
            chunk_id: int,
            progress: Dict
    ) -> bool:
        """
        下载文件块

        Args:
            url: 下载URL
            start: 起始字节
            end: 结束字节
            output_file: 输出文件路径
            chunk_id: 块ID
            progress: 进度字典
        """
        headers = {'Range': f'bytes={start}-{end}'}

        try:
            with self.client.stream('GET', url, headers=headers) as response:
                response.raise_for_status()

                # 定位到文件位置
                with open(output_file, 'rb+') as f:
                    f.seek(start)

                    for chunk in response.iter_bytes():
                        f.write(chunk)
                        # 更新进度
                        with progress['lock']:
                            progress['completed'] += len(chunk)

                return True

        except Exception as e:
            print(f"块 {chunk_id} 下载失败: {e}")
            return False

    def save_progress(self, url: str, chunks: list, file_size: int):
        """保存下载进度"""
        progress_data = {
            'url': url,
            'chunks': chunks,
            'file_size': file_size,
            'timestamp': time.time()
        }
        with open(self.progress_file, 'w') as f:
            for i, chunk in enumerate(chunks):
                f.write(f"{i}:{chunk[0]},{chunk[1]}\n")

    def load_progress(self, url: str) -> Optional[list]:
        """加载下载进度"""
        if not os.path.exists(self.progress_file):
            return None

        try:
            chunks = []
            with open(self.progress_file, 'r') as f:
                for line in f:
                    if ':' in line:
                        _, range_str = line.strip().split(':')
                        start, end = map(int, range_str.split(','))
                        chunks.append((start, end))
            return chunks if chunks else None
        except:
            return None

    def download(
            self,
            url: str,
            output_path: str = None,
            resume: bool = True
    ) -> bool:
        """
        下载文件

        Args:
            url: 下载URL
            output_path: 输出文件路径（默认为URL中的文件名）
            resume: 是否启用断点续传
        """
        # 获取输出文件名
        if output_path is None:
            output_path = url.split('/')[-1] or "download.file"

        # 获取文件大小
        file_size = self.get_file_size(url)
        if not file_size:
            return False

        print(f"文件大小: {file_size / 1024 / 1024:.2f} MB")

        # 检查已有进度
        chunks = []
        if resume:
            saved_chunks = self.load_progress(url)
            if saved_chunks:
                print("检测到未完成的下载，尝试续传...")
                chunks = saved_chunks
            else:
                # 创建新的分块
                chunk_size = file_size // self.max_workers
                for i in range(self.max_workers):
                    start = i * chunk_size
                    end = start + chunk_size - 1 if i < self.max_workers - 1 else file_size - 1
                    chunks.append((start, end))

        # 创建或打开文件
        if not os.path.exists(output_path):
            with open(output_path, 'wb') as f:
                f.truncate(file_size)

        # 进度跟踪
        progress = {
            'completed': 0,
            'total': file_size,
            'lock': threading.Lock()
        }

        # 进度条
        pbar = tqdm(total=file_size, unit='B', unit_scale=True, desc="下载进度")

        def update_progress():
            last_completed = 0
            while progress['completed'] < progress['total']:
                current = progress['completed']
                increment = current - last_completed
                if increment > 0:
                    pbar.update(increment)
                    last_completed = current
                time.sleep(0.1)
            pbar.close()

        # 启动进度更新线程
        progress_thread = threading.Thread(target=update_progress)
        progress_thread.start()

        # 保存进度
        self.save_progress(url, chunks, file_size)

        # 多线程下载
        with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            futures = []
            for i, (start, end) in enumerate(chunks):
                future = executor.submit(
                    self.download_chunk,
                    url, start, end, output_path, i, progress
                )
                futures.append(future)

            # 等待所有任务完成
            results = [f.result() for f in futures]

        # 等待进度条线程结束
        progress_thread.join()

        # 清理进度文件
        if os.path.exists(self.progress_file):
            os.remove(self.progress_file)

        success = all(results)
        if success:
            print(f"\n下载完成: {output_path}")
        else:
            print("\n下载失败")

        return success

    def close(self):
        """关闭客户端"""
        self.client.close()


# 使用示例
if __name__ == "__main__":
    # 创建下载器
    downloader = MultiThreadDownloader(max_workers=10)

    try:
        # 下载文件（支持断点续传）
        url = "https://twds.dl.sourceforge.net/project/mpv-player-windows/64bit-v3/mpv-x86_64-v3-20260104-git-a3350e2.7z?viasf=1"
        success = downloader.download(
            url=url,
            output_path="mpv-x86_64-v3-20260104-git-a3350e2.7z",
            resume=True
        )

        if success:
            print("下载成功！")
        else:
            print("下载失败，请检查网络或URL")

    except KeyboardInterrupt:
        print("\n下载被用户中断，进度已保存")
    except Exception as e:
        print(f"下载出错: {e}")
    finally:
        downloader.close()