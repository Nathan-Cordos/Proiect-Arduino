Proiectul consta intr-un robot arduino (o masinuta) care navigheaza de la un punct definit A la alt punct definit B (acestea sunt definite in program prin coordonate x,y), isi calculeaza si tine minte pozitia iar in timpul miscarii evita obstacole.

Pozitia este calculata si updatata folosind niste senzori optici si rotite perforate atasate axului de la motor; acestea impreuna actioneaza ca niste encoderi incrementali, desigur cu o precizie mult, mult mai scazuta.

Obstacolele sunt evitate folosind senzori cu ultra sunete, iar implementarea in cod folosete o retea neuronala.

Reteaua neuronala este excesiva in acest caz, se putea la fel de bine face cu niste if-uri. Scopul meu a fost sa invat mai multe despre retele neuronale, mai precis, am vrut sa inteleg cum functioneaza backpropagation-ul, cum se reduce eroarea.

Reteaua neuronala a fost antrenata separat de placa Arduino Uno pentru a minimiza costul implementarii pe microcontroller.

De mentionat, pentru codul de antrenare am urmat ghidul domnului John Bullinaria de la Universitatea Birmingham UK.

Aici este link-ul lucrarii domnului John Bullinaria: https://www.cs.bham.ac.uk/~jxb/INC/nn.html

Am studiat codul dansului, am facut adaptarile necesare si am implementat.
