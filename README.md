**Számítógépes grafika beadandó feladat**

- Készíts egy alkalmazást amiben létrehozol egy 5 egységkockából felépített Tetris X formát. A létrehozott geometria minden háromszögének legyen különböző színe!

- A fent elkészített geometriát rajzold ki a következő formációban: egy XY síkbeli origó középpontú, 5 egység sugarú körív mentén egyenletesen elhelyezett 5 pontban.

- Az így kirajzolt formációt mozgasd a következő pályán: egy XZ síkban fekvő z = 0.01*x^3 + 0.05*x^2 görbe -10<=x<=10 közötti részén a -10 és 10 közötti végpontok között oda-vissza mozogva, egy ilyen utat 6 másodperc alatt megtéve.

- A felhasználó a következőképpen állíthassa a színtérbeli történéseket: a geometriák 5 másodpercenként 1 másodpercig kezdjenek el gyors ütemben rezegni (ide oda forogni a saját tengelyük körül). Ha ekkor a felhasználó lenyomja a szóközt, a rezgő objektumok kezdjenek el egyre gyorsulva növekedni, míg mindent el nem fednek a képernyőn. Ez után álljon minden vissza az eredeti kerékvágásba. A kamerát a színtérbe úgy helyezd le, hogy látható legyen minden! 