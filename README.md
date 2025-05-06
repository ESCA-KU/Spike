# Spike
This project is a Functional Simulator project which includes gemmini extension, RISC-V V-extension, ROPE Custom instruction, Toknizer custom instruction

# Spike 실행 매뉴얼

Spike는 RISC-V ISA 시뮬레이터로, 다양한 확장 명령어를 지원하며 디버깅 기능도 제공합니다.  
이 문서는 Spike의 빌드 방법, 실행 명령어, 디버깅 사용법을 정리한 가이드입니다.

---

## Spike Build 방법
> $PATH_TO_SPIKE$는 git clone을 수행한 위치, 즉 Spike 폴더의 위치를 의미합니다. 실행환경에 맞게 수정하세요.

### Prerequisites

```bash
sudo apt-get install device-tree-compiler libboost-regex-dev libboost-system-dev
```

### Spike를 소스에서 빌드하려면 다음 명령어를 차례대로 실행하세요:

```bash
cd riscv-isa-sim
mkdir build && cd build
../configure --prefix=$PATH_TO_SPIKE$/riscv-tools     #prefix는 꼭 절대경로로 입력
make clean -j$(nproc)
make -j$(nproc)
make install
```

> `--with-isa=rv64gcv` 옵션은 Vector Extension 및 기타 확장 기능이 포함된 RISC-V 64bit 아키텍처를 의미합니다. 필요에 따라 다른 ISA를 지정할 수 있습니다.


---
Build 후에 생성된 spike 실행파일은 riscv-tools/bin에 위치

## PK(Proxy Kernel) Build 방법

### Prerequisites

- **RISC-V Cross Compiler (`riscv64-unknown-elf-gcc`)**  
  `pk`는 RISC-V 환경에서 실행될 바이너리이므로, 반드시 RISC-V용 크로스 컴파일러가 필요합니다.
-  [riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain.git)을 먼저 빌드하여 설치하세요:

### PK를 소스에서 빌드하려면 다음 명령어를 차례로 실행하세요:

```bash
cd riscv-pk
mkdir build && cd build
../configure --prefix=$PATH_TO_SPIKE$/riscv-tools --host=riscv64-unknown-elf --with-arch=rv64gc    #prefix는 꼭 절대경로로 입력
make clean -j$(nproc)
make -j$(nproc)
make install
```
---
Build 후에 생성된 pk 실행파일은 riscv-tools/riscv64-unknown-elf/bin에 위치

# Spike 실행 명령어

> Spike 실행파일(`spike`)과 런타임(`pk`)의 위치는 각각 `$PATH_TO_SPIKE$/riscv-tools/bin/spike` `$PATH_TO_SPIKE$/riscv-tools/riscv64-unknown-elf/bin/pk`에 위치합니다.
> 각각 $spike, $pk로 지칭하겠습니다.
> 실제 환경에 맞게 경로를 조정하세요  

### 1. V-extension 코드 실행(ROPE, Tokenizer)

```bash
$spike --isa=rv64gcv_zvl256b_zve64d_zicntr $pk 실행파일명
```

### ✅ vector-tokenize 실행 (주의사항 포함)

```bash
$spike --isa=rv64gcv_zvl256b_zve64d_zicntr $pk vector-tokenize -p $PATH_TO_SPIKE$/test/Tokenizer -f prompt.txt
```

> - `-p` 옵션: 입력 텍스트 파일의 경로
> - `-f` 옵션: 입력 텍스트 파일명 (예: `prompt.txt`)  
> - `id_to_token.txt`와 `prompt.txt`는 같은 디렉토리에 있어야 합니다.

---

### 2. Gemmini 코드 실행

#### (1) BareMetal용 실행파일

```bash
$spike --extension=gemmini 실행파일명(~-baremetal)
```

#### (2) pk 연동 실행파일

```bash
$spike --extension=gemmini $pk 실행파일명(~-pk)
```

---

### 3. 디버깅 모드 실행

명령어에 `-d` 옵션을 추가하면 디버깅 모드로 진입할 수 있습니다.

#### 예시: V-extension 디버깅

```bash
$spike -d --isa=rv64gcv_zvl256b_zve64d_zicntr $pk 실행파일명
```

#### 예시: Gemmini 디버깅

```bash
$spike -d --extension=gemmini 실행파일명   # 파일 유형에 따라 pk 추가
```

---

## Spike 디버깅 모드 사용법

디버깅 모드에 진입한 뒤 사용할 수 있는 명령어는 아래와 같습니다:

```text
reg 0                   # 모든 레지스터 값 확인
reg 0 a5                # a5 레지스터 값 확인
vreg 0                  # 벡터 레지스터 값 확인
until pc 0 10110000     # PC가 0부터 0x10110000까지 진행될 때까지 실행
r 1                     # 다음 명령어 1개 실행
mem 0 0x10110050        # Virtual address 0x10110050 메모리 값 확인
insn 0                  # 현재 명령어 확인
```

> 위 명령어들은 디버그 콘솔에 입력하면 즉시 실행됩니다.

---

## 참고자료

- Spike 공식 GitHub 저장소: [https://github.com/riscv-software-src/riscv-isa-sim](https://github.com/riscv-software-src/riscv-isa-sim)

---

## 디렉토리 구조 예시

```text
~/Spike/   
├── riscv-isa-sim            # Spike 빌드에 필요한 src 파일들이 위치한 폴더
├── riscv-tools              # Spike 빌드 결과물들을 저장하는 폴더
│   ├── riscv64-unknown-elf  # PK 빌드 결과물들을 저장하는 폴더
└── test                     # Spike test에 필요한 실행파일 폴더
    ├── Gemmini              # Gemmini 코드 및 실행파일
    ├── ROPE                 # ROPE custom Instruction으로 작성된 코드 및 실행파일
    └── Tokenizer            # Tokenizer custom Instruction으로 작성된 코드 및 실행파일
```

> 실행 중 필요한 파일들이 올바른 위치에 있어야 에러 없이 실행됩니다.
