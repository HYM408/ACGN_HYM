import json
import sqlite3
import threading
from pathlib import Path
from datetime import datetime, timezone, timedelta

_thread_local = threading.local()

def _get_connection():
    """获取当前线程的数据库连接"""
    if not hasattr(_thread_local, "connection"):
        data_dir = Path('data')
        data_dir.mkdir(parents=True, exist_ok=True)
        db_path = data_dir / 'data.db'
        _thread_local.connection = sqlite3.connect(
            str(db_path),
            check_same_thread=False,
            timeout=30
        )
        _thread_local.connection.row_factory = sqlite3.Row
        cursor = _thread_local.connection.cursor()
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS collection
            (
                subject_id INTEGER PRIMARY KEY,
                vol_status INTEGER,
                ep_status INTEGER,
                subject_type INTEGER,
                type INTEGER,
                rate INTEGER,
                subject_date TEXT,
                subject_name TEXT,
                subject_name_cn TEXT,
                subject_tags TEXT,
                subject_eps INTEGER,
                subject_volumes INTEGER,
                subject_images_common TEXT,
                updated_at TEXT
            );
        ''')
        cursor.execute('''
            CREATE INDEX IF NOT EXISTS idx_subject_type_type_updated
                ON collection (subject_type, type, updated_at DESC);
        ''')
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS episode_collection
            (
                subject_id INTEGER,
                id INTEGER,
                ep REAL,
                sort INTEGER,
                name TEXT,
                name_cn TEXT,
                episode_type INTEGER,
                collection_type INTEGER,
                created_at TEXT DEFAULT (datetime('now')),
                PRIMARY KEY(subject_id,id)
            );
        ''')
        cursor.execute('''
            CREATE INDEX IF NOT EXISTS idx_episode_subject_id
                ON episode_collection (subject_id);
        ''')
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS subjects
            (
                id INTEGER PRIMARY KEY,
                name TEXT NOT NULL,
                name_cn TEXT,
                date TEXT,
                total_episodes INTEGER,
                volumes INTEGER,
                summary TEXT,
                rating_rank INTEGER,
                rating_score REAL,
                rating_total INTEGER,
                collect INTEGER,
                on_hold INTEGER,
                dropped INTEGER,
                wish INTEGER,
                doing INTEGER, 
                tags TEXT,
                created_at TEXT DEFAULT (datetime('now'))
            );
        ''')
        _thread_local.connection.commit()
    return _thread_local.connection

def _simplify_tags(tags):
    """tags简化"""
    if not tags:
        return None
    simplified = sorted(
        [[t.get("count", 0), t.get("name", "").strip()] for t in tags if t.get("name")],
        key=lambda x: x[0],
        reverse=True
    )
    return json.dumps(simplified, ensure_ascii=False, separators=(',', ':'))

def _determine_subject_type(original_type, tags):
    """处理小说(7).漫画(8)subject_type"""
    if original_type != 1:
        return original_type
    if tags:
        for tag in tags:
            tag_name = tag.get("name", "").strip()
            if tag_name == "漫画":
                return 8
    return 7

def insert_many_data(data_list):
    """批量插入多条数据"""
    conn = _get_connection()
    cursor = conn.cursor()
    try:
        values = []
        for data in data_list:
            subject = data.get('subject', {})
            images = subject.get('images', {})
            full_url = images.get('common', '')
            original_subject_type = data.get('subject_type')
            tags = subject.get('tags', [])
            final_subject_type = _determine_subject_type(original_subject_type, tags)
            tags_compressed = _simplify_tags(tags)
            values.append((
                data.get('subject_id'),
                data.get('vol_status'),
                data.get('ep_status'),
                final_subject_type,
                data.get('type'),
                data.get('rate'),
                subject.get('date'),
                subject.get('name'),
                subject.get('name_cn'),
                tags_compressed,
                subject.get('eps'),
                subject.get('volumes'),
                full_url[37:] if full_url.startswith('https://lain.bgm.tv/r/400/pic/cover/l/') else full_url,
                data.get('updated_at')
            ))
        cursor.executemany('''INSERT OR REPLACE INTO collection (subject_id, vol_status, ep_status, subject_type, type, rate, subject_date, subject_name, subject_name_cn, subject_tags, subject_eps, subject_volumes, subject_images_common, updated_at)
        VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?)''', values)
        conn.commit()
    except Exception as e:
        conn.rollback()
        raise e

def get_by_subject_type_and_type(subject_type, type_value):
    """根据subject_type和type获取所有符合条件的记录，按updated_at降序排列"""
    conn = _get_connection()
    cursor = conn.cursor()
    cursor.execute('''
        SELECT subject_id,
               subject_name,
               subject_name_cn,
               subject_eps,
               subject_images_common,
               vol_status,
               type
        FROM collection
        WHERE subject_type = ?
          AND type = ?
        ORDER BY updated_at DESC
    ''', (subject_type, type_value))
    rows = cursor.fetchall()
    results = []
    for row in rows:
        result = dict(row)
        relative_path = result.get('subject_images_common', '')
        if relative_path:
            result['subject_images_common'] = f"https://lain.bgm.tv/r/400/pic/cover/l/{relative_path}"
        results.append(result)
    return results

def clear_table():
    """清空整个表"""
    conn = _get_connection()
    cursor = conn.cursor()
    cursor.execute('DELETE FROM collection')
    conn.commit()

def get_by_subject_id(subject_id):
    """根据subject_id获取单条记录"""
    conn = _get_connection()
    cursor = conn.cursor()
    cursor.execute('''
        SELECT subject_id,
               subject_name,
               subject_name_cn,
               subject_eps,
               subject_images_common,
               vol_status,
               type
        FROM collection 
        WHERE subject_id = ?
    ''', (subject_id,))
    row = cursor.fetchone()
    if not row:
        return None
    result = dict(row)
    result['subject_tags'] = json.loads(result['subject_tags']) if result.get('subject_tags') else []
    if result.get('subject_images_common'):
        result['subject_images_common'] = f"https://lain.bgm.tv/r/400/pic/cover/l/{result['subject_images_common']}"
    return result

def update_field(subject_id, field, value):
    """更新指定subject_id的某个字段值"""
    conn = _get_connection()
    cursor = conn.cursor()
    if field == 'subject_tags':
        value = json.dumps(value, ensure_ascii=False) if value else None
    cursor.execute(f'UPDATE collection SET {field} = ? WHERE subject_id = ?', (value, subject_id))
    current_time = datetime.now(timezone(timedelta(hours=8))).isoformat()
    cursor.execute('UPDATE collection SET updated_at = ? WHERE subject_id = ?', (current_time, subject_id))
    conn.commit()

def delete_by_subject_id(subject_id):
    """根据subject_id删除记录"""
    conn = _get_connection()
    cursor = conn.cursor()
    cursor.execute('DELETE FROM collection WHERE subject_id = ?', (subject_id,))
    conn.commit()

def cleanup():
    """清理数据库连接"""
    if hasattr(_thread_local, "connection"):
        try:
            _thread_local.connection.close()
            delattr(_thread_local, "connection")
        except:
            pass

# ===============剧集表函数===============
def insert_many_episodes(subject_id, episodes_list):
    """批量插入多条剧集数据"""
    conn = _get_connection()
    cursor = conn.cursor()
    try:
        values = []
        for episode_item in episodes_list:
            episode_data = episode_item.get('episode', {})
            values.append((
                subject_id,
                episode_data.get('id'),
                episode_data.get('ep'),
                episode_data.get('sort'),
                episode_data.get('name'),
                episode_data.get('name_cn'),
                episode_data.get('type'),  # episode字段下的type
                episode_item.get('type')   # 外层JSON的type字段
            ))
        cursor.executemany('''INSERT OR REPLACE INTO episode_collection (subject_id, id, ep, sort, name, name_cn, episode_type, collection_type)
        VALUES (?,?,?,?,?,?,?,?)
        ''', values)
        conn.commit()
    except Exception as e:
        conn.rollback()
        raise e

def get_episodes_by_subject_id(subject_id):
    """根据subject_id获取所有剧集，按sort升序排列"""
    conn = _get_connection()
    cursor = conn.cursor()
    cursor.execute('''
        SELECT * 
        FROM episode_collection 
        WHERE subject_id = ? 
        ORDER BY sort ASC
    ''', (subject_id,))
    rows = cursor.fetchall()
    return [dict(row) for row in rows]

def delete_episodes_by_subject_id(subject_id):
    """根据subject_id删除所有剧集记录"""
    conn = _get_connection()
    cursor = conn.cursor()
    cursor.execute('DELETE FROM episode_collection WHERE subject_id = ?', (subject_id,))
    conn.commit()

# ===============subject表函数===============
def insert_subject_from_api(api_data):
    """API插入subjects表"""
    conn = _get_connection()
    cursor = conn.cursor()
    try:
        subject_id = api_data.get('id', 0)
        name = api_data.get('name', '')
        name_cn = api_data.get('name_cn', '')
        date = api_data.get('date', '')
        total_episodes = api_data.get('total_episodes', 0)
        volumes = api_data.get('volumes', 0)
        summary = api_data.get('summary', '')
        rating = api_data.get('rating', {})
        rating_rank = rating.get('rank', 0)
        rating_score = rating.get('score', 0.0)
        rating_total = rating.get('total', 0)
        collect = api_data.get('collection').get('collect', 0)
        on_hold = api_data.get('collection').get('on_hold', 0)
        dropped = api_data.get('collection').get('dropped', 0)
        wish = api_data.get('collection').get('wish', 0)
        doing = api_data.get('collection').get('doing', 0)
        tags = api_data.get('tags', [])
        tags_json = _simplify_tags(tags)
        cursor.execute('''INSERT OR REPLACE INTO subjects (id, name, name_cn, date, total_episodes, volumes, summary, rating_rank, rating_score, rating_total, collect, on_hold, dropped, wish, doing, tags)
        VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)''', (subject_id, name, name_cn, date, total_episodes, volumes, summary, rating_rank, rating_score,rating_total, collect, on_hold, dropped, wish, doing, tags_json))
        conn.commit()
    except Exception as e:
        conn.rollback()
        print(f"插入subject失败: {e}")
        raise e

def get_subject_by_id(subject_id):
    """根据ID获取subject信息"""
    conn = _get_connection()
    cursor = conn.cursor()
    cursor.execute('SELECT * FROM subjects WHERE id = ?', (subject_id,))
    row = cursor.fetchone()
    if not row:
        return None
    result = dict(row)
    if result.get('tags'):
        try:
            result['tags'] = json.loads(result['tags'])
        except json.JSONDecodeError:
            pass
    return result

def update_all_episodes_status(subject_id, collection_type=2):
    """批量更新指定subject_id的所有剧集状态"""
    conn = _get_connection()
    cursor = conn.cursor()
    try:
        cursor.execute(
            'UPDATE episode_collection SET collection_type = ? WHERE subject_id = ?',
            (collection_type, subject_id)
        )
        conn.commit()
    except Exception as e:
        conn.rollback()
        raise e