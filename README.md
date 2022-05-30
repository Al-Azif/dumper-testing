# dumper-testing

Rewriting PS4 application dumper from scratch in C++. This repo will be deleted when a permanent repo is setup. Don't bother cloning unless you're going to be making immediate changes.

**No code here is finished or guaranteed to work.** Goal is for readability first to allow easier collaboration until everything is working 100%, don't want to optimize it too early:

1. Get it all working. For the first draft we will assume the asset is completely installed and is currently running on the system.
2. Refactor to make more sense.
3. More robust. Find/fix edge cases and have good, descriptive, error messages for issues that cannot be handled automatically.
4. Setup tests so we don't break anything trying to enhance performance.
5. Performance.

> "Move fast break things"

**ANYONE** can contribute. Code is currently licensed under GPLv3, by submitting a pull request you agree to this term and agree to possible relicensing later.

## Known Issues

- sc0 entry decryption (Like `npbind.dat`)*
- "New" binary decryption function crashes*
- Games that have multiple supported languages or multiple discs may not "install" all the files and the dumper will hang trying to copy it
  - Can also apparently happen if the disc is dirty and not installing correctly when you start the dump
  - Current workaround is to make sure you have all the languages and all the discs completely installed prior to dumping
  - Thoughts: Is there a PFS flag that mark these files? Or is there some other way to detect "optional" files?

\* These currently have work arounds in the dumper built into the homebrew store

## TODO

- [ ] Fix known issues listed above
- [X] NpBind
  - [X] Implementation
  - [X] Tests
- [ ] SFO
  - [X] Implementation
  - [ ] Tests
- [ ] GP4
  - [ ] Implementation
    - [X] Basic GP4 Generation
    - [ ] PlayGo Related Issues (Want to match retail PKG settings)
    - [ ] PFS Compress Option (Want to match retail PKG settings)
  - [ ] Tests
- [ ] PKG
  - [ ] Implementation
    - [X] Basic
    - [ ] Decrypt encrypted Sc0 entries
    - [ ] `is_pkg` and `is_fpkg` functions
  - [ ] Tests
- [ ] PFS
  - [X] Implementation
  - [ ] Tests
- [ ] Decrypt SELF
  - [X] Implementation
  - [ ] Tests
- [ ] FSELF
  - [ ] Implementation
    - Zero'd ELF header(?)
  - [ ] Tests
- [ ] RIF (For Additional Content w/o Data & Entitlement Keys)
  - [ ] Implementation
  - [ ] Tests
- [ ] Dump
  - [X] Base
  - [X] Patch
  - [X] Additional Content w/ Data
  - [X] Theme
  - [X] Retail Theme + Theme "Unlock"
  - [X] Remaster
    - [ ] Build a script to diff the actual files with the original content PC side to make a patch PKG vs a Remaster (?)
  - [ ] Additional Content w/o Data
  - [ ] Multi-Disc (Does it just work without changes?)
- [ ] Verification
  - [ ] Save encrypted/modified files for backup/preservation purposes
    - [X] Encrypted Binary (FILENAME.EXT.encrypted)
    - [X] Decrypted Binary (FILENAME.EXT)
    - [X] FSELF (FILENAME.fself)
    - [X] Encrypted sc0 entries (FILENAME.EXT.encrypted)
    - [ ] Decrypted sc0 entries (FILENAME.EXT)
  - [X] Check decrypted binaries vs hash in encrypted header
  - [X] SFV-like file to compare vs known good rips
  - [ ] External service to verify/submit rip data
    - [ ] Optional submission to database
    - [ ] No actual data downloaded/uploaded, only metadata
- [ ] Remove extra/unnecessary checks
  - ex. Calling `std::filesystem::is_regular_file` and `elf::is_self` together. Because `elf::is_self` calls `std::filesystem::is_regular_file` on the input itself
- [ ] Split into self contained, read/write, individual libraries to be used in other applications
- [ ] UI integration
  - Dump via dump options in main menu
  - Progress shown within notifications
  - Errors via error dialog
