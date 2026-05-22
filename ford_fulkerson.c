#include <stdio.h>
#include <limits.h>

struct Edges {
    int u;
    int v;
    int w;
};

struct Label {
    int v;
    int d;
    int type;
};

struct Edges U[100];
struct Label V[100];
int f[100][100], C[100][100];
int val_f;

//* Hàm đọc dữ liệu thô từ file input.txt dưới dạng danh sách cạnh
void read_input(int *n, int *m, int *s, int *t) {
    FILE *pF = fopen("input.txt","r");
    fscanf(pF, "%d", n);
    fscanf(pF, "%d", m);
    fscanf(pF, "%d", s);
    fscanf(pF, "%d", t);
    for (int i = 0; i < *m; i++) {
        fscanf(pF, "%d", &U[i].u);
        fscanf(pF, "%d", &U[i].v);
        fscanf(pF, "%d", &U[i].w);
    }
}

//* Hàm Data Preprocessing: Chuyển dữ liệu từ dạng Cạnh sang Ma trận kề để tối ưu O(1) khi truy xuất kề
void convert_to_matrix(int n, int m) {
    // 1. Xóa sạch ma trận C về 0 trước khi nạp dữ liệu
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
            f[i][j] = 0; // Sẵn tiện khởi tạo luồng bằng 0 luôn cho Bước 1
        }
    }

    // 2. Duyệt qua mảng cạnh U để đổ vào ma trận C
    for (int i = 0; i < m; i++) {
        int u = U[i].u;
        int v = U[i].v;
        int w = U[i].w;
        
        C[u][v] = w; // Đút thẳng vào ma trận 2 chiều
    }
}

//* Hàm khởi tạo ban đầu cho toàn bộ mạng lưới (chỉ chạy duy nhất 1 lần khi bắt đầu thuật toán)
void init(int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            f[i][j] = 0;
        }
    }
    for (int i = 0; i < n; i++) {
        V[i].v = -1;
        V[i].type = 0;
        V[i].d = 0;
    }
}

//* Hàm xóa nhãn cũ để chuẩn bị không gian trống cho một đợt loang BFS mới
void reset_label(int n) {
    for (int i = 0; i < n; i++) {
        V[i].v = -1;
        V[i].type = 0;
        V[i].d = 0;
    }
}

// Hàm tìm giá trị nhỏ hơn giữa 2 số nguyên
int find_min(int a, int b) {
    return a < b ? a : b;
}

//* Hàm lõi thực hiện điều phối dòng chảy kịch trần bằng phương pháp gán nhãn đường tăng luồng
void ford_fulkerson(int n, int m, int s, int t) {
    init(n);
    while(1) { // Vòng lặp tổng thể tìm đường tăng luồng liên tục cho đến khi tắc nghẽn hoàn toàn
        reset_label(n);

        V[s].v = s; //! LƯU Ý: Phải chốt V[s].v = s để bảo vệ đỉnh nguồn
        V[s].d = INT_MAX; // Đỉnh nguồn có khả năng cấp tải vô hạn
        V[s].type = 0; // Đỉnh xuất phát mặc định loại cung bằng 0

        // Khởi tạo Queue thủ công bằng mảng phục vụ chiến lược loang đều theo chiều rộng (BFS)
        int queue[100], front = 0, rear = 0;
        queue[rear++] = s; // Nạp nguồn s vào cuối hàng đợi để chuẩn bị kích hoạt làn sóng loang thứ nhất
        
        // Vòng lặp BFS: Chạy khi Queue còn phần tử xếp hàng VÀ đích t vẫn trần trụi chưa nhận được nhãn
        while (front < rear && V[t].v == -1) {
            int u = queue[front++]; // Bốc người đứng đầu hàng đợi ra làm tâm phát nhãn tiếp theo

            for (int v = 0; v < n; v++) {
                if(V[v].v == -1) { // Cơ chế Problem Solving: Chỉ đi xét và gán nhãn cho những đỉnh v thực sự trần trụi (chưa có nhãn) để giữ tính toàn vẹn của đường đi ngắn nhất, tránh tranh chấp nhãn làm đứt đoạn dò vết
                    
                    // Nhánh 1: Kiểm tra năng lực loang xuôi theo chiều mũi tên gốc u -> v
                    if (C[u][v] > 0 && f[u][v] < C[u][v]) {
                        V[v].v = u;
                        V[v].type = 1; // Ghi nhật ký: Đây là cung xuôi
                        V[v].d = find_min(V[u].d, C[u][v] - f[u][v]); // Thắt cổ chai bằng khoảng trống công suất còn lại
                        queue[rear++] = v; // Gán nhãn xong liền ném v vào cuối Queue chờ xếp hàng phát nhãn lượt sau
                    }
                    
                    // Nhánh 2: Kiểm tra năng lực loang ngược (đi lùi) ngược chiều mũi tên gốc v -> u
                    //! LƯU Ý KỸ THUẬT: Dùng khối IF độc lập (không dùng else if) để máy tính quét toàn bộ các cạnh kề, tránh bỏ sót trường hợp một đỉnh vừa có cung xuôi mở vừa có cung ngược mở sang đỉnh khác.
                    if (C[v][u] > 0 && f[v][u] > 0) {
                        //! BẢN CHẤT CUNG NGƯỢC: Điều kiện f[v][u] > 0 nghĩa là ống v -> u phải đang có nước chạy qua thì mới có thể đi lùi từ u về v nhằm mục đích "rút bớt luồng/thu hồi dịch vụ" để chuyển hướng sang ngả khác tối ưu hơn. Nếu f[v][u] == 0 (ống rỗng) thì không thể bóp nghẹt dòng chảy được nữa, năng lực tải ngược bằng 0.
                        V[v].v = u;
                        V[v].type = -1; // Ghi nhật ký: Đây là cung ngược (đi lùi để sửa sai)
                        V[v].d = find_min(V[u].d, f[v][u]); // Trần gánh tải đi lùi tối đa bằng chính lượng luồng thực tế hiện có trên cung gốc
                        queue[rear++] = v;
                    }
                }
            }
        } // Kết thúc một đợt loang khảo sát của Queue

        // Kiểm tra điều kiện dừng tối ưu của mạng lưới
        if (V[t].v == -1) {
            //! CÚ THOÁT XÍCH VÔ HẠN: Nếu loang hết sạch Queue mà đích t vẫn bằng -1, chứng tỏ toàn mạng lưới đã bị nghẽn kịch trần, không còn đường nào tăng luồng được nữa. Lệnh BREAK bẻ gãy vòng lặp while(1) ngay lập tức để xuống in kết quả.
            break;
        }

        // Bước cập nhật luồng thực tế (Chỉ chạy khi tìm được mạch thông suốt về đích t ở trên)
        int theta = V[t].d; // Lượng nước bơm thêm vào chính bằng thắt cổ chai nhỏ nhất tại đích t
        int curr = t;
        
        // Dò vết lùi từ đích t quay ngược lại nguồn s dựa vào mảng nhãn ghi nhớ V[curr].v
        while (V[curr].type != 0) {
            int dad = V[curr].v; // Lấy ra đỉnh cha (mắt xích trước) của đỉnh hiện tại
            
            if (V[curr].type == 1) {
                f[dad][curr] += theta; // Lượt loang khảo sát đi xuôi thì tiến hành CỘNG luồng
            }
            if (V[curr].type == -1) {
                //! TÁI CƠ CẤU LUỒNG: Lượt khảo sát đi lùi (cung ngược) thì tiến hành TRỪ luồng trên cung gốc f[curr][dad] nhằm mục đích bẻ thẳng dòng chảy, giải phóng công suất trống cho toàn đồ thị. Dòng chảy tổng về đích chỉ tăng hoặc giữ nguyên chứ không bao giờ giảm.
                f[curr][dad] -= theta; 
            }
            curr = dad; // Nhảy lùi tiếp về mắt xích phía trước cho đến khi đụng nguồn s (type == 0)
        }
    }
}

//* Hàm in báo cáo chi tiết kết quả điều phối dòng chảy cực đại lên màn hình Terminal
void print_result(int n, int m, int s) {
    // 1. Tính tổng luồng cực đại đi ra từ nguồn s
    val_f = 0;
    for (int v = 0; v < n; v++) {
        if (C[s][v] > 0) {
            val_f += f[s][v]; // Gom tất cả luồng thực tế chảy ra trực tiếp từ s trên ma trận f
        }
    }

    // 2. In tổng luồng cực đại lên màn hình
    printf("\n=============================================\n");
    printf("   KET QUA THUAT TOAN FORD-FULKERSON\n");
    printf("=============================================\n");
    printf(">>> TONG LUONG CUC DAI TIM DUOC: %d\n\n", val_f);

    // 3. In chi tiết luồng thực tế trên từng cạnh dựa theo mảng U ban đầu
    printf("Chi tiet dong chay tren cac canh (luong / cong suat):\n");
    for (int i = 0; i < m; i++) {
        int u = U[i].u;
        int v = U[i].v;
        int w = U[i].w; // Công suất gốc
        
        // Luồng thực tế chính là giá trị trong ma trận f[u][v] sau khi thuật toán kết thúc
        printf("Canh %d: (%d -> %d) | Luong thuc te: %d / %d\n", i + 1, u, v, f[u][v], w);
    }
    printf("=============================================\n");
}

int main() {
    int n, m, s, t;
    read_input(&n, &m, &s, &t);
    
    //! TIỀN XỬ LÝ QUAN TRỌNG: Bắt buộc phải gọi hàm này ngay sau khi đọc input để đổ dữ liệu từ mảng U vào ma trận C, nếu sót dòng này ma trận C toàn bộ bằng 0 và thuật toán sẽ thất bại ngay lập tức.
    convert_to_matrix(n, m);
    
    ford_fulkerson(n, m, s, t);
    print_result(n, m, s);
}
