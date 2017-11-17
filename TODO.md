#### TRUMAN ESCAPE ####
Easter Egg: attivabile solo dando una specifica keyword al nome del gioco: "Truman"
L'idea sarebbe quella di fare scappare Truman dal mondo costruito per tenerlo prigioniero. Si potrebbe costuire un mondo anche a diversi livelli, 
ovvero non solo con la sfera iniziale ma pure una sfera successiva o addirittura un cubo, queste saranno implementate tramite la draw_sphere, 
o draw_universe, dell'env.hxx.
Si potrebbe persino aggiungere uno stato del livello di avanzamento tra le sfere, cioè dopo aver completato gli anelli del primo mondo 
si potrebbe uscire appunto dalla sfera grossa e spawnare questi anelli pure nel mondo esterno.

### Final Gate
Precaricato con una mesh; si attiva quando finiscono gli anelli. Una volta superato questo ultima porta, si disegna una sfera più grande esterna con una 
texture di un mondo vario e colorato. Altrimenti appena la si attraversa finisce il gioco, si può fare anche così. 
La posizione se non erro dovrebbe essere Z = Floor::SIZE e X = 0 tipo

### Multiple Spaceships : flappyShip
- variabili private --> protected 
- metodi virtual, e quando implementati marcarli con override 
- virtual: updatePosition, updateFly, computePhysics e via dicendo 
- get_spaceship(bool flappy3D = false); sarà la nuova signature; prende bool e ritorna un'istanza di una delle due spaceship 
- tutto dipende da una variable bool, quindi bisogna fare un check su init_game e anche su quando si fa restartGame bisogna riassegnare il puntatore
  ad una nuova spaceship a seconda della Texture e della tipologia di volo. Dopodiché dovremmo essere veramente al completo 

### 3D flight mode: elementi
- variabili private --> protected 
- Anelli avranno una componente Y che li spawnerà in alto
- Il check invece sarà esattamente uguale perché tanto non devono mai essere attraversati in verticale, sono come cerchi in aria

### Menu: 
- ombre e lights dovrebbero essere nel menù
- aggiungere envmapping che funziona bene e wireframe


