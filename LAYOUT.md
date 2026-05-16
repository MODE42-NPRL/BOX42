BOX42/
│
├── box42                 # Hauptbinary (Server)
├── box42-node            # Node/AX.25 Binary
│
├── src/                  # Alle Quellen (C)
│   ├── core/
│   ├── net/
│   ├── session/
│   ├── cmd/
│   ├── subsystems/
│   ├── storage/
│   ├── util/
│   └── include/
│
├── build/                # Build-Artefakte (obj, dep, temp)
│   ├── obj/
│   ├── dep/
│   └── logs/
│
├── bin/                  # Kopien der fertigen Binaries
│   ├── box42
│   └── box42-node
│
├── etc/                  # Konfiguration
│   ├── box42.conf
│   ├── logging.conf
│   ├── sshd.conf
│   ├── tnc2c.conf
│   ├── modem.conf
│   ├── usb.conf
│   └── sqlite.conf
│
├── data/                 # Persistente Datenbanken & Logs
│   ├── users.db
│   ├── mail.db
│   ├── node.db
│   ├── config.db
│   ├── logs/
│   │   ├── box42.log
│   │   ├── box42.log.1
│   │   └── …
│   └── mail/
│
├── tools/                # Hilfsskripte
│   ├── mkb64split.sh
│   ├── mkzip.sh
│   ├── keygen.sh
│   ├── usb-scan.sh
│   └── dev-clean.sh
│
└── LAYOUT.md             # Diese Datei
