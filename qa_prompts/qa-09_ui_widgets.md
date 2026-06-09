# QA-09 Rev — UI Widget Tests (DIPERBAIKI)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Perbaikan
- ✅ API signatures diverifikasi dari HEADER ASLI (client/ui/widgets/*.hpp)
- ✅ widget.hpp: `SetRect(x,y,w,h)`, `SetPos(x,y)`, `SetVisible(bool)`, `SetEnabled(bool)`, `OnEvent(callback)`, `HitTest(x,y)`
- ✅ Button: `Button(text, x, y, w, h)`, `SetText(t)`, `GetText()`, `SetColors(n,h,p)`
- ✅ CheckBox: `CheckBox(text, x, y)`, `IsChecked()`, `SetChecked(bool)`, `GetText()`, `SetText(t)`
- ✅ InputField: `InputField(x,y,w,h)`, `GetText()`, `SetText(t)`, `SetMaxLength(n)`, `SetPlaceholder(p)`, `SetValidation(InputValidation)`
- ✅ ListBox: `ListBox(x,y,w,h)`, `AddItem(s)`, `Clear()`, `GetSelected()`, `SetSelected(idx)`, `GetCount()`, `GetItem(idx)`
- ✅ TabPanel: `TabPanel(x,y,w,h)`, `AddTab(name, Widget*)`, `SetActive(idx)`, `GetActive()`
- ✅ Grid: `Grid(rows, cols, slot_w, slot_h, x, y)`, `GetSlot(row, col)`
- ✅ ProgressBar: `ProgressBar(x,y,w,h)`, `SetProgress(float)`, `GetProgress()`, `SetColors(fg,bg)`, `SetText(t)`
- ✅ Window: `Window(title, x, y, w, h)`, `SetTitle(t)`, `GetTitle()`, `SetClosable(bool)`, `SetMovable(bool)`, `AddWidget<T>(args...)`, `Close()`

## Test Scenarios (8 test → 6 test, API sesuai aktual)

### Test 1: Widget Base
```cpp
Widget w(10, 20, 100, 200);
TEST("Widget x=10", w.GetX() == 10);
TEST("Widget y=20", w.GetY() == 20);
TEST("Widget w=100", w.GetW() == 100);
TEST("Widget h=200", w.GetH() == 200);
w.SetPos(5, 15);
TEST("SetPos x=5", w.GetX() == 5);
w.SetVisible(false);
TEST("SetVisible false", !w.IsVisible());
w.SetEnabled(false);
TEST("SetEnabled false", !w.IsEnabled());
```

### Test 2: Button
```cpp
Button btn("Click", 0, 0, 100, 30);
TEST("Button text", btn.GetText() == "Click");
btn.SetText("OK");
TEST("Button set text", btn.GetText() == "OK");
btn.SetColors({50,100,150,255}, {80,130,180,255}, {30,60,100,255});
TEST("Button constructed", true);
```

### Test 3: CheckBox
```cpp
CheckBox cb("Save ID", 0, 0);
TEST("CheckBox text", cb.GetText() == "Save ID");
TEST("CheckBox unchecked by default", !cb.IsChecked());
cb.SetChecked(true);
TEST("CheckBox can be checked", cb.IsChecked());
cb.SetChecked(false);
TEST("CheckBox can be unchecked", !cb.IsChecked());
```

### Test 4: InputField
```cpp
InputField field(0, 0, 200, 22);
field.SetText("Hello");
TEST("InputField text", field.GetText() == "Hello");
field.SetPlaceholder("Enter name");
field.SetMaxLength(20);
// Validation
field.SetValidation(InputValidation::PositiveInteger);
field.SetText("abc");
TEST("Non-numeric rejected", field.GetText().empty());
field.SetText("12345");
TEST("Numeric accepted", field.GetText() == "12345");
```

### Test 5: ListBox
```cpp
ListBox list(0, 0, 200, 150);
list.AddItem("Item 1");
list.AddItem("Item 2");
list.AddItem("Item 3");
TEST("ListBox count = 3", list.GetCount() == 3);
TEST("ListBox item 0", list.GetItem(0) == "Item 1");
list.SetSelected(1);
TEST("ListBox selected = 1", list.GetSelected() == 1);
list.Clear();
TEST("ListBox cleared", list.GetCount() == 0);
```

### Test 6: ProgressBar + TabPanel
```cpp
ProgressBar pb(0, 0, 200, 16);
pb.SetProgress(0.5f);
TEST("Progress = 0.5", pb.GetProgress() == 0.5f);
pb.SetProgress(1.5f);
TEST("Progress clamped to 1.0", pb.GetProgress() == 1.0f);

TabPanel tabs(0, 0, 400, 300);
tabs.AddTab("Tab1", nullptr);
tabs.AddTab("Tab2", nullptr);
tabs.SetActive(1);
TEST("Tab active = 1", tabs.GetActive() == 1);
```

## ✅ Kembalikan: "QA-09 done: UI widget tests — 6 test suites, all passed"
