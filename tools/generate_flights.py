"""
航班数据生成器
生成 10000 条覆盖中国主要机场的航班数据
"""

import sqlite3
import random
from datetime import datetime, timedelta
import os

# ==================== 中国主要机场数据 ====================
AIRPORTS = [
    # 一线城市
    ("北京", "北京首都国际机场", "PEK"),
    ("北京", "北京大兴国际机场", "PKX"),
    ("上海", "上海浦东国际机场", "PVG"),
    ("上海", "上海虹桥国际机场", "SHA"),
    ("广州", "广州白云国际机场", "CAN"),
    ("深圳", "深圳宝安国际机场", "SZX"),
    
    # 新一线/二线城市
    ("成都", "成都天府国际机场", "TFU"),
    ("成都", "成都双流国际机场", "CTU"),
    ("重庆", "重庆江北国际机场", "CKG"),
    ("杭州", "杭州萧山国际机场", "HGH"),
    ("南京", "南京禄口国际机场", "NKG"),
    ("武汉", "武汉天河国际机场", "WUH"),
    ("西安", "西安咸阳国际机场", "XIY"),
    ("昆明", "昆明长水国际机场", "KMG"),
    ("长沙", "长沙黄花国际机场", "CSX"),
    ("郑州", "郑州新郑国际机场", "CGO"),
    ("青岛", "青岛胶东国际机场", "TAO"),
    ("厦门", "厦门高崎国际机场", "XMN"),
    ("天津", "天津滨海国际机场", "TSN"),
    ("沈阳", "沈阳桃仙国际机场", "SHE"),
    ("大连", "大连周水子国际机场", "DLC"),
    ("哈尔滨", "哈尔滨太平国际机场", "HRB"),
    ("长春", "长春龙嘉国际机场", "CGQ"),
    ("济南", "济南遥墙国际机场", "TNA"),
    ("福州", "福州长乐国际机场", "FOC"),
    ("合肥", "合肥新桥国际机场", "HFE"),
    ("南昌", "南昌昌北国际机场", "KHN"),
    ("太原", "太原武宿国际机场", "TYN"),
    ("石家庄", "石家庄正定国际机场", "SJW"),
    ("南宁", "南宁吴圩国际机场", "NNG"),
    ("贵阳", "贵阳龙洞堡国际机场", "KWE"),
    ("海口", "海口美兰国际机场", "HAK"),
    ("三亚", "三亚凤凰国际机场", "SYX"),
    ("兰州", "兰州中川国际机场", "LHW"),
    ("乌鲁木齐", "乌鲁木齐地窝堡国际机场", "URC"),
    ("呼和浩特", "呼和浩特白塔国际机场", "HET"),
    ("银川", "银川河东国际机场", "INC"),
    ("西宁", "西宁曹家堡国际机场", "XNN"),
    ("拉萨", "拉萨贡嘎国际机场", "LXA"),
    
    # 其他重要城市
    ("无锡", "苏南硕放国际机场", "WUX"),
    ("宁波", "宁波栎社国际机场", "NGB"),
    ("温州", "温州龙湾国际机场", "WNZ"),
    ("珠海", "珠海金湾国际机场", "ZUH"),
    ("汕头", "揭阳潮汕国际机场", "SWA"),
    ("烟台", "烟台蓬莱国际机场", "YNT"),
    ("威海", "威海大水泊国际机场", "WEH"),
    ("桂林", "桂林两江国际机场", "KWL"),
    ("丽江", "丽江三义国际机场", "LJG"),
    ("西双版纳", "西双版纳嘎洒国际机场", "JHG"),
    ("张家界", "张家界荷花国际机场", "DYG"),
    ("九寨沟", "九寨黄龙机场", "JZH"),
    ("敦煌", "敦煌莫高国际机场", "DNH"),
]

# 航空公司代码
AIRLINES = [
    ("CA", "中国国际航空"),
    ("MU", "中国东方航空"),
    ("CZ", "中国南方航空"),
    ("HU", "海南航空"),
    ("ZH", "深圳航空"),
    ("MF", "厦门航空"),
    ("3U", "四川航空"),
    ("FM", "上海航空"),
    ("SC", "山东航空"),
    ("GS", "天津航空"),
    ("KN", "中国联合航空"),
    ("9C", "春秋航空"),
    ("HO", "吉祥航空"),
    ("8L", "祥鹏航空"),
    ("G5", "华夏航空"),
]


def generate_flight_id(airline_code: str, index: int) -> str:
    """生成航班号"""
    return f"{airline_code}{1000 + index}"


def get_flight_duration(dep_city: str, arr_city: str) -> int:
    """根据城市估算飞行时间（分钟）"""
    # 简化：根据是否跨区域估算时长
    long_distance_cities = {"乌鲁木齐", "拉萨", "哈尔滨", "三亚", "海口"}
    
    if dep_city in long_distance_cities or arr_city in long_distance_cities:
        return random.randint(180, 300)  # 3-5小时
    elif dep_city in {"北京", "上海", "广州"} and arr_city in {"北京", "上海", "广州"}:
        return random.randint(120, 180)  # 2-3小时
    else:
        return random.randint(90, 180)  # 1.5-3小时


def get_price(duration: int, is_hot_route: bool) -> float:
    """根据飞行时长和热门程度计算价格"""
    base_price = duration * 3  # 基础：每分钟3元
    if is_hot_route:
        base_price *= random.uniform(1.2, 1.8)
    else:
        base_price *= random.uniform(0.8, 1.2)
    return round(base_price + random.randint(-100, 200), 0)


def generate_flights(num_flights: int = 10000):
    """生成航班数据"""
    flights = []
    flight_index = 0
    
    # 热门航线（一线城市之间）
    hot_cities = {"北京", "上海", "广州", "深圳", "成都", "杭州", "重庆", "西安"}
    
    # 生成未来60天的航班
    start_date = datetime.now().replace(hour=0, minute=0, second=0, microsecond=0)
    
    while len(flights) < num_flights:
        # 随机选择出发和到达机场
        dep_airport = random.choice(AIRPORTS)
        arr_airport = random.choice(AIRPORTS)
        
        # 跳过同城航班
        if dep_airport[0] == arr_airport[0]:
            continue
        
        # 随机选择航空公司
        airline = random.choice(AIRLINES)
        
        # 随机日期（未来60天内）
        day_offset = random.randint(0, 59)
        flight_date = start_date + timedelta(days=day_offset)
        
        # 随机出发时间（6:00 - 22:00）
        hour = random.randint(6, 21)
        minute = random.choice([0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55])
        depart_time = flight_date.replace(hour=hour, minute=minute)
        
        # 计算飞行时长和到达时间
        duration = get_flight_duration(dep_airport[0], arr_airport[0])
        arrive_time = depart_time + timedelta(minutes=duration)
        
        # 计算价格
        is_hot = dep_airport[0] in hot_cities and arr_airport[0] in hot_cities
        price = get_price(duration, is_hot)
        
        # 座位数（必须是6的倍数，适配前端座位选择界面：每排6座）
        # 可选值：120, 132, 144, 156, 168, 180（即20-30排）
        seats = random.choice([120, 132, 144, 156, 168, 180])
        
        # 生成航班号
        flight_id = generate_flight_id(airline[0], flight_index)
        flight_index += 1
        
        flights.append({
            "flight_id": flight_id,
            "departure": dep_airport[0],
            "destination": arr_airport[0],
            "departure_airport": dep_airport[1],
            "arrival_airport": arr_airport[1],
            "depart_time": depart_time.strftime("%Y-%m-%dT%H:%M:%S"),
            "arrive_time": arrive_time.strftime("%Y-%m-%dT%H:%M:%S"),
            "price": price,
            "rest_seats": seats
        })
    
    return flights


def insert_to_database(db_path: str, flights: list, clear_existing: bool = True):
    """将航班数据插入 SQLite 数据库"""
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    
    if clear_existing:
        cursor.execute("DELETE FROM flight")
        print(f"✓ 已清空原有航班数据")
    
    insert_sql = """
        INSERT INTO flight (flight_id, departure, destination, departure_airport, 
                           arrival_airport, depart_time, arrive_time, price, rest_seats)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    """
    
    for i, f in enumerate(flights):
        try:
            cursor.execute(insert_sql, (
                f["flight_id"], f["departure"], f["destination"],
                f["departure_airport"], f["arrival_airport"],
                f["depart_time"], f["arrive_time"],
                f["price"], f["rest_seats"]
            ))
        except sqlite3.IntegrityError:
            # 航班号重复，跳过
            pass
        
        if (i + 1) % 1000 == 0:
            print(f"  已插入 {i + 1} 条...")
    
    conn.commit()
    
    # 统计结果
    cursor.execute("SELECT COUNT(*) FROM flight")
    total = cursor.fetchone()[0]
    
    cursor.execute("SELECT COUNT(DISTINCT departure) FROM flight")
    cities = cursor.fetchone()[0]
    
    conn.close()
    
    print(f"\n✅ 数据导入完成！")
    print(f"   - 总航班数: {total}")
    print(f"   - 覆盖城市: {cities}")


def main():
    print("=" * 50)
    print("  FTMS 航班数据生成器")
    print("=" * 50)
    
    # 数据库路径（默认在 build 目录）
    script_dir = os.path.dirname(os.path.abspath(__file__))
    default_db = os.path.join(script_dir, "..", "build", 
                              "Desktop_Qt_6_9_3_MinGW_64_bit-Debug", "ftms.db")
    
    print(f"\n默认数据库路径: {os.path.abspath(default_db)}")
    db_path = input("请输入数据库路径 (直接回车使用默认): ").strip()
    
    if not db_path:
        db_path = default_db
    
    if not os.path.exists(db_path):
        print(f"\n❌ 数据库文件不存在: {db_path}")
        print("请先运行一次后端服务器以创建数据库，然后再运行此脚本。")
        return
    
    print(f"\n正在生成 10000 条航班数据...")
    flights = generate_flights(10000)
    print(f"✓ 已生成 {len(flights)} 条航班数据")
    
    print(f"\n正在导入数据库...")
    insert_to_database(db_path, flights)
    
    print("\n" + "=" * 50)
    print("  完成！现在可以启动系统测试了")
    print("=" * 50)


if __name__ == "__main__":
    main()
