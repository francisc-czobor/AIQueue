import java.util.ArrayList;
import java.util.List;

public class MyBot {
	
    private final static InitPackage iPackage = Networking.getInit();
    private final static int myID = iPackage.myID;
    private final static GameMap gameMap = iPackage.map;
	
	private static ArrayList<Site> neighbors(Location location) {
		ArrayList<Site> neighbors = new ArrayList<Site>(4);

		neighbors.add(gameMap.getSite(location, Direction.CARDINALS[0]));
		neighbors.add(gameMap.getSite(location, Direction.CARDINALS[1]));
		neighbors.add(gameMap.getSite(location, Direction.CARDINALS[2]));
		neighbors.add(gameMap.getSite(location, Direction.CARDINALS[3]));
		return neighbors;
	}

	private static Move assignMove(Location location) {
		ArrayList<Site> neighbors = neighbors(location);

		for (int i = 0; i < 4; i++) {
			if (neighbors.get(i).owner != myID
					&& neighbors.get(i).strength < location.getSite().strength) {
				return new Move(location, Direction.CARDINALS[i]);
			}
		}
		
		if (location.getSite().strength < 10 * location.getSite().production) {
			return new Move(location, Direction.STILL);
		} else {
			return new Move(location, Direction.randomDirection());
		}
	}
	
    public static void main(String[] args) throws java.io.IOException {
        Networking.sendInit("AIQueue_francisc");

        while(true) {
            List<Move> moves = new ArrayList<Move>();

            Networking.updateFrame(gameMap);

            for (int y = 0; y < gameMap.height; y++) {
                for (int x = 0; x < gameMap.width; x++) {
                    final Location location = gameMap.getLocation(x, y);
                    final Site site = location.getSite();
                    if(site.owner == myID) {
                        moves.add(assignMove(location));
                    }
                }
            }
            Networking.sendFrame(moves);
        }
    }
}
