#### Abstract Graphic Library : AGL
AGL deve essere l'astrazione della libreria grafica, quindi è definita da un header, in cui c'è un namespace dove sono dichiarate tutte le funzioni che servono. Queste funzioni verranno poi definite nei var .cxx a seconda dei moduli che hanno bisogno della tale funzione.

#### TRUMAN ESCAPE ####
L'idea sarebbe quella di fare scappare Truman dal mondo costruito per tenerlo prigioniero. Si potrebbe costuire un mondo anche a diversi livelli, ovvero non solo con la sfera iniziale ma pure una sfera successiva o addirittura un cubo, queste saranno implementate tramite la draw_sphere, o draw_universe, dell'env.hxx.
Si potrebbe persino aggiungere uno stato del livello di avanzamento tra le sfere, cioè dopo aver completato gli anelli del primo mondo si potrebbe uscire appunto dalla sfera grossa e spawnare questi anelli pure nel mondo esterno.

#### Gli Anelli
Gli anelli dovrebbero se possibile contenere una scritta in alto? fattibile? Oppure in basso che compare in sovraimpressione ad ogni anello che si supera. Queste scritte ovviamente sono statiche e precaricate e coincidono con i pezzi della storia. Si indicano i vari pezzi della storia. L'incontro con la tipa di cui si innamora, le litigate con la moglie, scoprire che anche il suo migliore amico gli mente, ecc.

### Final Gate
Precaricato con una mesh; si attiva quando finiscono gli anelli. Una volta superato questo ultima porta, si disegna una sfera più grande esterna con una texture di un mondo vario e colorato. 

### Multiple Spaceships 
- variabili private --> protected 
- metodi virtual, e quando implementati marcarli con override 
- virtual: updatePosition, updateFly, computePhysics e via dicendo 
- get_spaceship(bool flappy3D = false); sarà la nuova signature; prende bool e ritorna un'istanza di una delle due spaceship 

### 3D flight mode: 
Anelli avranno una componente Y che li spawnerà in alto 

### Menu: 
- possibilità di cambiare spaceship ==> causa il restart del gioco 
- ombre e lights dovrebbero essere nel menù
- quando si ricomincia controllare la variabile "game started"
- quando si quitta farlo con invio

### Ranking: 
Il nome inserito come parametro in start_game sarà inserito in un file di testo ranking.txt che verrà stampato a schermo al termine del gioco. 
Questo ranking conterrà il tempo totale. 
