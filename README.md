Thuật Toán Luồng Cực Đại (Max Flow) - Ford-Fulkerson

Dự án triển khai thuật toán Ford-Fulkerson tìm luồng cực đại trên mạng bằng cấu trúc dữ liệu **Ma trận kề** kết hợp giải thuật **Loang theo chiều rộng (BFS)** tối ưu bằng **Queue**.

## 👥 Thành viên thực hiện
* **Đỗ Hữu Minh Trí** - Lớp 25T_KHDL - Trường Đại học Bách khoa, Đại học Đà Nẵng

## 🚀 Tính năng & Giải thuật cốt lõi
* **Biểu diễn đồ thị:** Sử dụng Ma trận kề để quản lý công suất cạnh $C[u][v]$ và luồng hiện tại $f[u][v]$.
* **Tìm đường tăng luồng:** Áp dụng thuật toán BFS (Queue) để tìm đường đi ngắn nhất từ nguồn $s$ đến đích $t$ trên đồ thị tăng cường.
* **Tối ưu hóa bộ nhớ:** Triển khai cấu trúc Queue vòng phẳng giúp tăng tốc độ loang nhãn và giảm chi phí cấp phát dữ liệu.

---

## 💡 Tư duy giải thuật & Lưu ý đặc biệt (Ghi chú chuyên sâu)

### 1. Cơ chế chống loang ngược về nguồn
Trong quá trình loang BFS, việc khóa đỉnh nguồn bằng cách gán nhãn `V[s].v = s` (khác `-1`) là bắt buộc. Điều này ngăn các đỉnh con loang ngược về nguồn gây ra hiện tượng **Vòng lặp vô hạn** trong logic mã nguồn.

### 2. Hiện tượng nghẽn mạng kịch trần (Cú thoát Break tối ưu)
Khi mạng lưới đạt trạng thái tối ưu tuyệt đối, tất cả các đường từ $s$ đến $t$ đều bị nghẽn (Công suất bằng Luồng: $C[u][v] == f[u][v]$). 
* **Hệ quả:** BFS không thể loang nhãn tới đích t, khiến mảng nhãn giữ nguyên trạng thái khởi tạo `V[t].v == -1`.
* **Xử lý:** Lệnh `break` sẽ kích hoạt ngay lập tức để chặn việc lặp vô hạn, giữ nguyên ma trận luồng tối ưu và kết thúc thuật toán hoàn hảo.

---

## 📂 Cấu trúc thư mục
* `ford_fulkerson.c`: Mã nguồn chính chứa toàn bộ thuật toán và hệ thống comment phân màu logic.
* `input.txt`: File chứa bộ dữ liệu test mẫu (Số đỉnh, số cạnh, danh sách các cạnh và công suất).

## 🛠️ Hướng dẫn chạy chương trình
1. Đảm bảo máy tính đã cài đặt trình biên dịch GCC.
2. Mở Terminal tại thư mục dự án và chạy lệnh biên dịch:
   ```bash
   gcc ford_fulkerson.c -o ford_fulkerson
