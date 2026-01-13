import asyncio
import aiohttp
import aiofiles
import os
import argparse
from urllib.parse import urljoin
from typing import List, Optional
from pathlib import Path


class M3U8Downloader:
    def __init__(self, max_concurrent: int = 10):
        """
        初始化下载器
        :param max_concurrent: 最大并发下载数
        """
        self.max_concurrent = max_concurrent
        self.session = None

    async def __aenter__(self):
        """异步上下文管理器入口"""
        self.session = aiohttp.ClientSession()
        return self

    async def __aexit__(self, exc_type, exc_val, exc_tb):
        """异步上下文管理器退出"""
        if self.session:
            await self.session.close()

    async def download_segment(self, url: str, index: int, output_dir: str,
                               semaphore: asyncio.Semaphore) -> Optional[str]:
        """
        下载单个TS片段
        """
        async with semaphore:
            try:
                filename = os.path.join(output_dir, f"segment_{index:05d}.ts")

                async with self.session.get(url, timeout=30) as response:
                    if response.status == 200:
                        async with aiofiles.open(filename, 'wb') as f:
                            await f.write(await response.read())
                        print(f"✓ 已下载: segment_{index:05d}.ts")
                        return filename
                    else:
                        print(f"✗ 下载失败 {url}: HTTP {response.status}")

            except Exception as e:
                print(f"✗ 下载失败 {url}: {str(e)}")
            return None

    def parse_m3u8(self, content: str, base_url: str) -> List[str]:
        """
        解析M3U8文件，获取TS片段URL列表
        """
        segments = []
        lines = content.strip().split('\n')

        for line in lines:
            line = line.strip()
            # 跳过注释和空行
            if not line or line.startswith('#'):
                continue
            # 如果是相对URL，转换为绝对URL
            if not line.startswith(('http://', 'https://')):
                segment_url = urljoin(base_url, line)
            else:
                segment_url = line
            segments.append(segment_url)

        return segments

    async def download_m3u8(self, m3u8_url: str, output_dir: str = "output") -> str:
        """
        下载M3U8文件及其所有TS片段
        """
        # 创建输出目录
        Path(output_dir).mkdir(parents=True, exist_ok=True)

        print(f"📥 开始下载 M3U8: {m3u8_url}")

        # 下载M3U8文件
        async with self.session.get(m3u8_url) as response:
            m3u8_content = await response.text()

        # 保存M3U8文件
        m3u8_filename = os.path.join(output_dir, "playlist.m3u8")
        async with aiofiles.open(m3u8_filename, 'w', encoding='utf-8') as f:
            await f.write(m3u8_content)

        # 获取基础URL用于拼接相对路径
        base_url = m3u8_url.rsplit('/', 1)[0] + '/'

        # 解析TS片段
        segments = self.parse_m3u8(m3u8_content, base_url)
        print(f"📊 找到 {len(segments)} 个TS片段")

        # 创建信号量控制并发
        semaphore = asyncio.Semaphore(self.max_concurrent)

        # 创建下载任务
        tasks = []
        for i, segment_url in enumerate(segments):
            task = self.download_segment(segment_url, i, output_dir, semaphore)
            tasks.append(task)

        # 并发下载所有片段
        downloaded_files = await asyncio.gather(*tasks)

        # 合并所有TS文件
        output_file = os.path.join(output_dir, "output.ts")
        print(f"\n🔄 开始合并TS文件...")

        with open(output_file, 'wb') as outfile:
            for i, filename in enumerate(downloaded_files):
                if filename and os.path.exists(filename):
                    with open(filename, 'rb') as infile:
                        outfile.write(infile.read())
                    # 可选：删除临时文件
                    os.remove(filename)

        print(f"✅ 下载完成！保存至: {output_file}")
        print(f"📏 文件大小: {os.path.getsize(output_file) / (1024 * 1024):.2f} MB")

        return output_file

    def convert_to_mp4(self, ts_file: str, output_file: Optional[str] = None) -> str:
        """
        使用FFmpeg将TS文件转换为MP4（如果系统已安装FFmpeg）
        """
        if not output_file:
            output_file = ts_file.replace('.ts', '.mp4')

        try:
            import subprocess
            cmd = [
                'ffmpeg', '-i', ts_file,
                '-c', 'copy',  # 直接复制流，不重新编码
                '-bsf:a', 'aac_adtstoasc',  # 修复AAC音频
                output_file
            ]

            print(f"🎬 正在转换为MP4格式...")
            subprocess.run(cmd, check=True, capture_output=True)
            print(f"✅ 转换完成！保存至: {output_file}")

            # 删除原始TS文件
            os.remove(ts_file)

            return output_file

        except (ImportError, subprocess.CalledProcessError, FileNotFoundError):
            print("⚠️  未找到FFmpeg，跳过MP4转换")
            return ts_file


async def main():
    """主函数"""
    parser = argparse.ArgumentParser(description='M3U8下载器')
    parser.add_argument('url', help='M3U8文件URL')
    parser.add_argument('-o', '--output', default='output', help='输出目录')
    parser.add_argument('-c', '--concurrent', type=int, default=500,
                        help='并发下载数（默认: 500）')
    parser.add_argument('-m', '--mp4', action='store_true',
                        help='下载后转换为MP4格式（需要FFmpeg）')

    args = parser.parse_args()

    # 创建下载器
    async with M3U8Downloader(max_concurrent=args.concurrent) as downloader:
        try:
            # 下载M3U8
            ts_file = await downloader.download_m3u8(args.url, args.output)

            # 如果需要转换为MP4
            if args.mp4:
                downloader.convert_to_mp4(ts_file)

        except Exception as e:
            print(f"❌ 下载失败: {str(e)}")


if __name__ == "__main__":
    # 运行异步主函数
    asyncio.run(main())