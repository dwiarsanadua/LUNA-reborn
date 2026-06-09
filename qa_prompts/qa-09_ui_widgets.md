# QA-09 — Visual & Render Test: UI Widget Rendering Validation

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Tugas

Buat `tools/test_runner/ui_widgets.cpp` — test validasi UI widget rendering (tanpa GLFW window — cukup validasi state/konfigurasi widget).

## Aturan Ketat

1. ✅ Panggil method yang SUDAH ADA
2. JANGAN buka window — test cukup sampai konstruksi widget + set properties
3. Build + run — 0 failure

## Test Scenarios

### Test 1: Widget Base Properties
```
TEST_STEP("Widget: position, size, visibility");
```
- Buat Widget generic
- SetRect(10, 20, 100, 200)
- Test: GetX() = 10, GetY() = 20, GetW() = 100, GetH() = 200
- SetVisible(false) → IsVisible() = false
- Test: semua property getter berfungsi

Cari dengan:
```bash
ls client/ui/widgets/Widget.*
```

### Test 2: Button Construct + Events
```
TEST_STEP("Button: construct, set text, set colors, fire event");
```
- Buat Button("Click Me", 0, 0, 100, 30)
- Test: button != nullptr
- Test: text tersimpan (cek label internal)
- SetColors(r, g, b) → color tersimpan
- OnEvent → FireEvent(Click) → callback terpanggil

### Test 3: InputField Validation
```
TEST_STEP("InputField: text input, validation, masking");
```
- Buat InputField
- SetText("Hello") → GetText() = "Hello"
- SetValidation(PositiveInteger) → SetText("abc") → text kosong
- SetText("123") → text = "123"
- SetMasked(true) → is_masked = true

### Test 4: CheckBox Toggle
```
TEST_STEP("CheckBox: toggle state, label, event on change");
```
- Buat CheckBox("Save ID")
- IsChecked() = false
- Toggle → IsChecked() = true
- FireEvent(ValueChanged) → callback fires

### Test 5: ListBox Items
```
TEST_STEP("ListBox: add items, select, clear");
```
- Buat ListBox
- AddItem("Item 1"), AddItem("Item 2")
- GetItemCount() = 2
- Select(0) → GetSelected() = 0
- Clear() → GetItemCount() = 0

### Test 6: TabPanel Tabs
```
TEST_STEP("TabPanel: add tabs, switch active, event on switch");
```
- Buat TabPanel
- AddTab("Tab1", grid1), AddTab("Tab2", list)
- GetActive() = 0
- SetActive(1) → GetActive() = 1
- FireEvent(TabSelected) → int_value = 1

### Test 7: Grid Slot Management
```
TEST_STEP("Grid: set items, slot events, scroll");
```
- Buat Grid(4, 5, ...)  → rows=4, cols=5
- SetItem(0, 0, icon_data) → GetItem(0,0) valid
- OnSlotEvent → click fires callback with row/col

### Test 8: Window Frame (Title Bar, Drag, Close)
```
TEST_STEP("Window: chrome, title, drag, close callback");
```
- Buat Window("Test", 100, 100, 400, 300)
- GetTitle() = "Test"
- SetClosable(true) → closable = true
- SetMovable(true) → movable = true
- OnClose → Close() → callback fires

## Output

✅ Kembalikan: "QA-09 done: UI widget tests — X passed, all properties validated"
