# BioButterfly_ESP — Solution Architecture Design

**Trạng thái / Status:** DESIGN_DRAFT — chờ Product Owner duyệt. **Rules-first baseline update:** 2026-11-23, owner đã chọn gamepad ESP-NOW làm đường điều khiển chính.  
**Người phụ trách / Owner:** Solution Architect (SA).  
**Khởi tạo / Date:** 2026-11-23.

## Mục tiêu

Thiết kế bướm robot sử dụng **ESP32 gamepad (TX) -> ESP-NOW -> XIAO ESP32-C3 (RX)** điều khiển hai cánh bằng SDS1601, hệ thống bảo vệ khi mất kết nối và giao diện web tùy chọn sau cho cấu hình/chẩn đoán và quy trình kiểm thử phần cứng an toàn. Ưu tiên thiết kế có thể kiểm chứng từng bước thay vì sao chép toàn bộ dự án tham khảo.

**Chưa duyệt thiết kế = chưa giao task code.** Các giả định phần cứng vẫn phải kiểm chứng trên thiết bị thực tế.

## Bộ tài liệu thiết kế

| File | Nội dung |
| --- | --- |
| [01_REQUIREMENTS.md](01_REQUIREMENTS.md) | Product requirements, MVP, acceptance, NFR |
| [02_UI_UX.md](02_UI_UX.md) | Mobile pilot cockpit, calibration UI, protocol examples |
| [03_SYSTEM_ARCHITECTURE.md](03_SYSTEM_ARCHITECTURE.md) | Firmware components, safety supervisor, state machine |
| [04_HARDWARE_AND_SAFETY.md](04_HARDWARE_AND_SAFETY.md) | Hardware layout, LiPo/servo safety, bench test gates |
| [05_AI_WORK_RULES.md](05_AI_WORK_RULES.md) | SA and AI Code roles, branch/commit rules, PR workflow |
| [06_DECISIONS_AND_ROADMAP.md](06_DECISIONS_AND_ROADMAP.md) | ADRs, roadmap, review ledger, design approval gates |
| [07_REFERENCE_AUDIT.md](07_REFERENCE_AUDIT.md) | Findings from original firmware and Google Drive research |
| **[AGENTS.md](AGENTS.md)** | **Điểm bắt đầu bắt buộc cho mọi AI: role, thứ tự đọc, reporting** |
| [08_MULTI_AGENT_RULES.md](08_MULTI_AGENT_RULES.md) | Quy tắc phối hợp SA, AI-3D, AI-HW và AI-SW; interface và handoff |
| [09_SENIOR_3D_AGENT.md](09_SENIOR_3D_AGENT.md) | Senior AI thiết kế cơ khí, cánh, CAD và in 3D |
| [10_SENIOR_HARDWARE_AGENT.md](10_SENIOR_HARDWARE_AGENT.md) | Senior AI điện tử, nguồn, mạch, PCB và safety |
| [11_SENIOR_SOFTWARE_AGENT.md](11_SENIOR_SOFTWARE_AGENT.md) | Senior AI firmware ESP32, UI, điều khiển và tests |
| **[12_AS_BUILT_INPUTS_AND_BLOCKERS.md](12_AS_BUILT_INPUTS_AND_BLOCKERS.md)** | **Baseline thực tế, hash STL/Gerber, những kết quả đã xác minh và blocker** |
| [13_ESP_NOW_DUAL_FIRMWARE_RULES.md](13_ESP_NOW_DUAL_FIRMWARE_RULES.md) | Rules ESP-NOW, gamepad TX/bướm RX, STOP, pairing, mất sóng |
| [14_ARMV5_HORN_AND_CARBON_FIT_RULES.md](14_ARMV5_HORN_AND_CARBON_FIT_RULES.md) | Rules sửa ARMV5, horn nhựa in 3D, 2 ốc và lỗ carbon 1.5mm |
| [15_PCB_V11_ASSEMBLY_AND_POWER_RULES.md](15_PCB_V11_ASSEMBLY_AND_POWER_RULES.md) | Rules kiểm tra Gerber PCB 4 lớp, linh kiện, pin/sạc và an toàn nguồn |
| **[16_HORN_3HOLE_2BOLT_PRETASK_PLAN.md](16_HORN_3HOLE_2BOLT_PRETASK_PLAN.md)** | **Owner đã chốt horn in hoàn toàn nhựa, gắn trực tiếp spline SDS1601; 3 lỗ thẳng hàng, 2 lỗ ARM, 2 ốc M1.6 dự kiến** |
| **[17_BAMBU_A1_25T_SPLINE_PRINT_VALIDATION.md](17_BAMBU_A1_25T_SPLINE_PRINT_VALIDATION.md)** | **25T Ø4,9mm + Bambu Lab A1: kiểm tra nozzle/material, in spline coupon và gate độ bền trước khi in horn thật** |
| **[18_OPENSCAD_CUSTOMIZER_25T_REVIEW.md](18_OPENSCAD_CUSTOMIZER_25T_REVIEW.md)** | **Đã chốt P7: 3 lỗ horn thẳng hàng cách 3,5mm, 2 lỗ ngoài cách 7mm; preset Futaba 3F Ø5,92 không phù hợp SDS1601 Ø4,9** |
| **[19_HORN_EXISTING_SPLINE_INTERFACE.md](19_HORN_EXISTING_SPLINE_INTERFACE.md)** | **Owner đã có phần spline cơ khí; tiếp tục thiết kế horn nhựa, phân biệt trục servo có răng với đầu nối spline rời để chọn lỗ tâm chính xác** |

## Đội ngũ AI chuyên trách

- **SA (tôi):** điều phối thiết kế liên ngành, quản lý rules, tạo Task sau phê duyệt, review và comment PR.
- **AI-3D:** thiết kế cơ cấu cánh/vỏ/frame, dựng CAD, đánh giá khối lượng và dung sai in.
- **AI-HW:** định nghĩa sơ đồ mạch, nguồn, pin/sạc/bảo vệ, giao diện servo và điện tử.
- **AI-SW:** phát triển ESP32 firmware, local web UI, state-machine và kiểm thử.

Mỗi AI phải bắt đầu bằng **[Design/AGENTS.md](AGENTS.md)**, sau đó đọc rules chung, role-specific rules và **file 12–18 liên quan trước khi thay đổi thiết kế**. Chúng là các **vai trò để giao cho những AI agent**; hiện không có ba phiên AI độc lập tự động chạy. Mọi AI chuyên môn chỉ triển khai sau khi được SA giao Task theo design đã duyệt. Không sửa chéo artifact của nhau.

## Quy trình làm việc

1. SA đọc nguồn tham khảo và chỉ cập nhật Design/ trong giai đoạn design. Mỗi thay đổi cần commit trên GitHub.
2. Product Owner duyệt phạm vi MVP, architecture, UI, phần cứng và quy tắc làm việc. SA ghi lại phiên bản thiết kế được duyệt bằng commit SHA.
3. **Chỉ sau khi design được duyệt**, SA tạo file Task/TASK-NNN-*.md quy định AI Code phải đọc gì, sửa file nào, tiêu chí nào và các bằng chứng test cần nộp.
4. AI-3D, AI-HW hoặc AI-SW được giao task riêng, làm việc trên nhánh riêng và mở PR. SA đọc Task + Design và review độc lập trong ReviewPR/PR-N, không thay đổi working tree của AI Code.
5. SA post comment review lên PR. Nếu có BLOCKER/MAJOR: giữ checkout + task, bổ sung điểm cần sửa. Nếu không có blocker: review có thể APPROVE; dọn riêng checkout và xóa file task được giao (sau khi lưu vết PR + quyết định trong design ledger), theo 05_AI_WORK_RULES.md.
6. Product Owner quyết định merge. Không suy luận có thể bay chỉ từ kết quả build phần mềm.

## Đã xác minh từ file bạn vừa cung cấp

- **Có thật theo chủ sở hữu:** XIAO ESP32-C3, gamepad ESP32, SDS1601, hai pin 3.7V 220mAh, PCB Gerber V1.1, ARMV5 đã in và FC Mount V5 chưa in.
- **Có thật theo dữ liệu file:** ARMV5 STL chứa 2 chi tiết lưới kín, FC Mount V5 chứa 1; Gerber có 4 lớp đồng, outline khoảng 18.5 x 45mm. Dữ liệu 3D/PCB chưa chứng minh fit hoặc an toàn cấp nguồn.
- **Owner đã chốt concept horn:** in nhựa 3D nguyên horn, trực tiếp lắp trục SDS1601, 3 lỗ thẳng hàng trên horn, 2 lỗ khung cánh tương ứng để gắn 2 ốc, lỗ giữa dự phòng. **Owner đã xác nhận spline 25T/Ø4,9mm và máy Bambu Lab A1**; vẫn phải đo biên dạng răng và in coupon kiểm tra, **không** dùng lỗ tròn SG90 thay spline. Cần biết nozzle đang lắp và vật liệu. 3 trong 4 lỗ carbon của ARM in vẫn chưa vừa; PCB chưa xác nhận BOM/schematic/nguồn.
- **Điều khiển M0:** gamepad ESP-NOW là kế hoạch chủ sở hữu; UI WebSocket trên điện thoại trong design cũ hiện được xem như ý tưởng cũ, KHÔNG là nguồn điều khiển chính.
- **File nguồn gốc** (STL/Gerber) chỉ được đính kèm ở cuộc trò chuyện, **chưa có trong repository**; các AI cần xin lại file/hashes, không đoán đường dẫn.

**Tình trạng:** RULES_UPDATED / DESIGN_DRAFT / NO_TASK / NO_IMPLEMENTATION.

## Lưu ý quan trọng về ảnh tùy chỉnh horn mới

Mã [mezger/customizable-servo-arm](https://github.com/mezger/customizable-servo-arm) được kiểm tra trực tiếp: preset Futaba 3F **25T Ø5,92mm**, khác spline SDS1601 **25T Ø4,90mm** (owner đã xác nhận). Preset SG90 trong cùng mã là **21T Ø4,90mm**, cũng không phù hợp. **Phải tạo spline SDS1601 tùy chỉnh** sau khi đo biên dạng và in coupon; không dùng ngay preset từ ảnh.

Thông số **Arm hole distance=6mm** trong ảnh là khoảng cách **hai lỗ liên tiếp**, nên giữa hai lỗ ngoài (dùng 2 ốc) là **12mm**; không giống proposal cũ **7mm**. **Owner đã chọn phương án B/P7 (ngày 2026-11-24)**: khoảng cách hai lỗ kế tiếp **3,5mm**, hai lỗ ngoài dùng để bắt **2 ốc** cách nhau **7mm**. ARMV5 có **2 lỗ trùng H1/H3**. P12 từ ảnh không được chọn. Vẫn cần xác minh vị trí H1 so với hub, độ dày và dung sai khi in trên Bambu A1. Chi tiết Design/18. Chưa sản xuất STL.

## Nguồn tham khảo

- [ESP-FLUTTER firmware by Max Imagination](https://github.com/maximagination1/ESP-FLUTTER) — README nêu giấy phép **PolyForm Noncommercial 1.0.0**. Quyền sử dụng thương mại/nhập lại source và asset phải xét riêng.
- [Google Drive: ESP-FLUTTER project files](https://drive.google.com/drive/folders/1XTTW8Ez_hmZl5E6mPUDXgX2qnnI3Cp2e).

Google Drive tham khảo **đã truy cập và kiểm tra được** các nhóm Datasheets, Wiring Diagrams, Wing Files, App Screenshots, PCB Files và danh mục tài liệu khác. Đã xem hình datasheet servo SDS1601, sơ đồ đấu nối modular, schematic PCB, bản vẽ hai cánh và bảng độ dài thanh carbon. Tuy nhiên, **chưa xác minh bất kỳ mạch và thông số phần cứng nào trên bản build thực tế của chúng ta**, chưa kiểm tra chi tiết Gerber/PCB assembly, chưa xác nhận đo dòng servo hay an toàn bộ sạc.

**Điểm kỹ thuật cần xác minh:** tài liệu wiring, PCB schematic và firmware tham khảo có những giá trị chia áp đo pin khác nhau; phạm vi PWM tham khảo cũng rộng hơn dải xung quảng cáo trong datasheet SDS1601. Không được dùng trực tiếp các thông số này mà không kiểm tra.

## Current gate

**DESIGN_DRAFT / OWNER_SA_DESIGN_APPROVAL_REQUIRED.** Đã cập nhật rules theo phần cứng hiện có; chưa có file Task/, chưa có AI Code PR, chưa sửa STL/Gerber/firmware. Chi tiết điểm chờ duyệt ở [06_DECISIONS_AND_ROADMAP.md](06_DECISIONS_AND_ROADMAP.md).
