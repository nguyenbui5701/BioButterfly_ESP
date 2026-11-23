# BioButterfly_ESP — Solution Architecture Design

**Trạng thái / Status:** DESIGN_DRAFT — chờ Product Owner duyệt.  
**Người phụ trách / Owner:** Solution Architect (SA).  
**Khởi tạo / Date:** 2026-11-23.

## Mục tiêu

Thiết kế bướm robot sử dụng ESP32 điều khiển hai cánh bằng servo, giao diện điều khiển trên điện thoại, hệ thống bảo vệ khi mất kết nối và quy trình kiểm thử phần cứng an toàn. Ưu tiên thiết kế có thể kiểm chứng từng bước thay vì sao chép toàn bộ dự án tham khảo.

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

## Quy trình làm việc

1. SA đọc nguồn tham khảo và chỉ cập nhật Design/ trong giai đoạn design. Mỗi thay đổi cần commit trên GitHub.
2. Product Owner duyệt phạm vi MVP, architecture, UI, phần cứng và quy tắc làm việc. SA ghi lại phiên bản thiết kế được duyệt bằng commit SHA.
3. **Chỉ sau khi design được duyệt**, SA tạo file Task/TASK-NNN-*.md quy định AI Code phải đọc gì, sửa file nào, tiêu chí nào và các bằng chứng test cần nộp.
4. AI Code làm việc trên nhánh riêng và mở PR. SA đọc Task + Design và review độc lập trong ReviewPR/PR-N, không thay đổi working tree của AI Code.
5. SA post comment review lên PR. Nếu có BLOCKER/MAJOR: giữ checkout + task, bổ sung điểm cần sửa. Nếu không có blocker: review có thể APPROVE; dọn riêng checkout và xóa file task được giao (sau khi lưu vết PR + quyết định trong design ledger), theo 05_AI_WORK_RULES.md.
6. Product Owner quyết định merge. Không suy luận có thể bay chỉ từ kết quả build phần mềm.

## Nguồn tham khảo

- [ESP-FLUTTER firmware by Max Imagination](https://github.com/maximagination1/ESP-FLUTTER) — README nêu giấy phép **PolyForm Noncommercial 1.0.0**. Quyền sử dụng thương mại/nhập lại source và asset phải xét riêng.
- [Google Drive: ESP-FLUTTER project files](https://drive.google.com/drive/folders/1XTTW8Ez_hmZl5E6mPUDXgX2qnnI3Cp2e).

Google Drive tham khảo **đã truy cập và kiểm tra được** các nhóm Datasheets, Wiring Diagrams, Wing Files, App Screenshots, PCB Files và danh mục tài liệu khác. Đã xem hình datasheet servo SDS1601, sơ đồ đấu nối modular, schematic PCB, bản vẽ hai cánh và bảng độ dài thanh carbon. Tuy nhiên, **chưa xác minh bất kỳ mạch và thông số phần cứng nào trên bản build thực tế của chúng ta**, chưa kiểm tra chi tiết Gerber/PCB assembly, chưa xác nhận đo dòng servo hay an toàn bộ sạc.

**Điểm kỹ thuật cần xác minh:** tài liệu wiring, PCB schematic và firmware tham khảo có những giá trị chia áp đo pin khác nhau; phạm vi PWM tham khảo cũng rộng hơn dải xung quảng cáo trong datasheet SDS1601. Không được dùng trực tiếp các thông số này mà không kiểm tra.

## Current gate

**DESIGN_DRAFT / OWNER_SA_DESIGN_APPROVAL_REQUIRED.** Chưa có file Task/ và chưa có AI Code PR. Chi tiết điểm chờ duyệt ở [06_DECISIONS_AND_ROADMAP.md](06_DECISIONS_AND_ROADMAP.md).
