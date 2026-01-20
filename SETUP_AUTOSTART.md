# Backend Automaattinen Käynnistys

## PM2 Asennus ja Konfigurointi

### 1. Asenna PM2 globaalisti
```bash
sudo npm install -g pm2
```

### 2. Käynnistä backend PM2:lla
```bash
cd /path/to/your/backend
pm2 start ecosystem.config.js
```

### 3. Tallenna PM2 prosessilista
```bash
pm2 save
```

### 4. Luo systemd service automaattista käynnistystä varten
```bash
pm2 startup systemd
```
Tämä komento näyttää komennon, joka pitää ajaa sudo-oikeuksilla. Aja se komento.

### 5. Tarkista että palvelu toimii
```bash
pm2 status
pm2 logs backend
```

## Käyttöohjeet

### Uusien tiedostojen jälkeen (git pull)
```bash
cd /path/to/your/backend
git pull
pm2 reload backend --update-env
```

Tai jos haluat nollan downtime:
```bash
pm2 reload ecosystem.config.js
```

### Sovelluksen pysäytys
```bash
pm2 stop backend
```

### Sovelluksen uudelleenkäynnistys
```bash
pm2 restart backend
```

### Lokit
```bash
pm2 logs backend          # Seuraa logeja reaaliajassa
pm2 logs backend --lines 100  # Näytä viimeiset 100 riviä
```

### PM2 dashboard
```bash
pm2 monit  # Reaaliaikainen seuranta
```

## Automatisoi git pull + reload

Voit luoda yksinkertaisen skriptin:

```bash
#!/bin/bash
# update-and-reload.sh

cd /path/to/your/backend
git pull origin main
npm install  # Jos riippuvuuksia päivitetty
pm2 reload backend --update-env
```

Tee skriptistä ajettava:
```bash
chmod +x update-and-reload.sh
```

## Automaattinen git pull cronilla

Voit ajastaa repositorion automaattisen päivityksen cron-tehtävänä.

### 1. Luo päivitysskripti

Luo skripti esim. `/yourpath/auto-update.sh`:

```bash
#!/bin/bash
# auto-update.sh

LOG_FILE="/yourpath/git-pull.log"
REPO_PATH="/path/to/your/repo"

echo "=== $(date) ===" >> "$LOG_FILE"
cd "$REPO_PATH" || exit 1

# Tarkista nykyinen branch
CURRENT_BRANCH=$(git branch --show-current)
echo "Nykyinen branch: $CURRENT_BRANCH" >> "$LOG_FILE"

# Vaihda main branchiin jos ei jo siellä
if [ "$CURRENT_BRANCH" != "main" ]; then
    echo "Vaihdetaan main branchiin..." >> "$LOG_FILE"
    git checkout main >> "$LOG_FILE" 2>&1
fi

# Hae muutokset
git fetch origin >> "$LOG_FILE" 2>&1

# Tarkista onko muutoksia
LOCAL=$(git rev-parse HEAD)
REMOTE=$(git rev-parse origin/main)

if [ "$LOCAL" != "$REMOTE" ]; then
    echo "Päivityksiä löytyi, suoritetaan git pull..." >> "$LOG_FILE"
    git pull origin main >> "$LOG_FILE" 2>&1
    
    # Päivitä backend riippuvuudet tarvittaessa
    cd backend
    npm install >> "$LOG_FILE" 2>&1
    
    # Uudelleenkäynnistä PM2
    pm2 reload backend --update-env >> "$LOG_FILE" 2>&1
    echo "Backend päivitetty ja uudelleenkäynnistetty" >> "$LOG_FILE"
else
    echo "Ei päivityksiä" >> "$LOG_FILE"
fi
```

Tee skriptistä ajettava:
```bash
chmod +x /yourpath/auto-update.sh
```

### 2. Lisää cron-tehtävä

Avaa crontab:
```bash
crontab -e
```

Lisää rivi ajamaan skripti esim. joka päivä klo 03:00:
```
0 3 * * * /yourpath/auto-update.sh
```

Tai joka tunti:
```
0 * * * * /yourpath/auto-update.sh
```

Tai joka 15. minuutti:
```
*/15 * * * * /yourpath/auto-update.sh
```

### 3. Tarkista cron-tehtävät
```bash
crontab -l
```

### 4. Seuraa lokeja
```bash
tail -f /yourpath/git-pull.log
```



