# OpenGL HW

## 1. Environment

* OS: Linux Ubuntu 22.04
* Compiler: g++ 11.4.0, C++17
* Library: freeglut, glew, glm

## 2. Build & Run

### 2.1. Install Vcpkg

```bash 
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
```

### 2.2. Install Library

First, install necessary library.
```bash
sudo apt-get install libxi-dev libgl1-mesa-dev libglu1-mesa-dev mesa-common-dev libxrandr-dev libxxf86vm-dev -y
```

Then, install freeglut, glew, glm.
```bash
./vcpkg/vcpkg install freeglut glew glm
```

### 2.3. Build

```bash
./build.sh
```

### 2.4. Run

```bash
./build/bin/CG2023_HW
```

## 3. Change details

### 3.1. Change raw pointer to smart pointer

```diff
- TriangleMesh* mesh = nullptr;
+ std::unique_ptr<TriangleMesh> mesh = nullptr;
```

```diff
- mesh = new TriangleMesh();
+ mesh = std::make_unique<TriangleMesh>();
```

### 3.2. Change string path to c++17 filesystem::path

```diff
- std::string path = "../models/Bunny.obj";
+ auto modelPath = std::filesystem::path("../models/Bunny.obj");
```

## 4. Commit message

feat: 新增/修改功能 (feature)  
fix: 修補 bug (bug fix)  
docs: 文件 (documentation)  
style: 格式 (不影響程式碼運行的變動 white-space, formatting, missing semi colons, etc)  
refactor: 重構 (既不是新增功能，也不是修補 bug 的程式碼變動)  
perf: 改善效能 (A code change that improves performance)  
test: 增加測試 (when adding missing tests)  
chore: 建構程序或輔助工具的變動 (maintain)  
revert: 撤銷回覆先前的 commit 例如：revert: type(scope): subject (回覆版本：xxxx)  