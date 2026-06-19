# RISC-V Monitor sa flat fajl sistemom

Jednostavan sistemski program (monitor) sa integrisanim flat fajl sistemom, razvijen za **RISC-V** arhitekturu (64-bitni profil, RV64). Program se izvršava neposredno na procesoru, bez operativnog sistema — tzv. *bare-metal* okruženje. Komunikacija sa korisnikom odvija se preko serijskog interfejsa (UART), a celokupno okruženje se emulira pomoću QEMU emulatora.

Projekat je organizovan u dva sloja: **monitor**, koji omogućava čitanje, upis i izvršavanje proizvoljnih lokacija u radnoj memoriji, i **flat fajl sistem**, koji nad istom memorijom uvodi apstrakciju imenovanih fajlova sa dinamičkom alokacijom blokova.

---

## Mogućnosti

Monitor podržava neposredan rad sa memorijom, dok fajl sistem dodaje upravljanje imenovanim fajlovima.

**Komande monitora**

| Komanda | Opis |
| :-----: | ---- |
|   `r`   | Čitanje i heksadecimalni ispis bajtova sa zadate adrese |
|   `w`   | Upis bajtova na zadatu adresu |
|   `x`   | Izvršavanje koda na zadatoj adresi |
|   `t`   | Ispis dela memorije u tekstualnom (ASCII) obliku |

**Komande fajl sistema**

|  Komanda  | Opis |
|  :-----:  | ---- |
| `c <ime>` | Kreiranje novog fajla |
|    `l`    | Ispis svih fajlova |
| `s <ime> <tekst>` | Upis sadržaja u fajl |
| `p <ime>` | Ispis sadržaja fajla |
| `d <ime>` | Brisanje fajla |
| `m <ime> <adresa>` | Kopiranje fajla na zadatu adresu u memoriji |
| `e <ime>` | Izvršavanje fajla |
| `a <ime>` | Ispis adrese fajla u memoriji |

---

## Pokretanje

Za izgradnju i pokretanje potrebni su RISC-V toolchain (`riscv64-unknown-elf-*`), `qemu-system-riscv64` i `make`.

```bash
cd src
make qemu
```

Komanda prevodi izvorni kod i pokreće monitor u QEMU emulatoru. Po pokretanju se javlja prompt `>` koji čeka na komande.

---

## Tehnologije

`C` · `RISC-V Assembly` · `QEMU` · `GNU toolchain (gcc, ld, as)` · `GDB` · `Make`

---

## Dokumentacija

Detaljan opis arhitekture, implementacije i dizajnerskih odluka nalazi se u tehničkoj dokumentaciji (docs/Dokumentacija.docx).
