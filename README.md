Proiectul consta intr-un robot arduino (o masinuta) care navigheaza de la un punct definit A la alt punct definit B (acestea sunt definite in program prin coordonate x,y), isi calculeaza si tine minte pozitia iar in timpul miscarii evita obstacole.

Pozitia este calculata si updatata folosind niste senzori optici si rotite perforate atasate axului de la motor; acestea impreuna actioneaza ca niste encoderi incrementali, desigur cu o precizie mult, mult mai scazuta. In program numararea de impulsuri este implementata folosind intreruperi (PIN Change intrerrupts) pe grupurile DI 9-13 si A0-A5.

Obstacolele sunt evitate folosind senzori cu ultra sunete, iar implementarea in cod folosete o retea neuronala.

Reteaua neuronala este excesiva in acest caz, se putea la fel de bine face cu niste if-uri. Scopul meu a fost sa invat mai multe despre retele neuronale, mai precis, am vrut sa inteleg cum functioneaza backpropagation-ul, cum se reduce eroarea.

Reteaua neuronala a fost antrenata separat de placa Arduino Uno pentru a minimiza costul implementarii pe microcontroller.

De mentionat, pentru codul de antrenare am urmat ghidul domnului John Bullinaria de la Universitatea Birmingham UK.

Aici este link-ul lucrarii domnului John Bullinaria: https://www.cs.bham.ac.uk/~jxb/INC/nn.html

Am studiat codul dansului, am facut adaptarile necesare si am implementat.

Performanta robotului este scazuta din cauza echipamentului imprecis si structura mecanica.

Apar erori de odometrie din cauza rezolutiei extrem de scazuta a "encoderului".

Numarul senzorilor cu ultrasunete nu este suficient si felul in care sunt montati creeaza multe unghiuri moarte in care masina nu depisteaza obstacole.

Chiar daca tensiunea furnizata motoarelor dc este aceeasi cateodata unul se invarte mai repede ca altul ceea ce face ca masina sa curbeze putin si sa nu mentina o directie corespunzatoare.
                                              
