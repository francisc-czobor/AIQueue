import java.util.ArrayList;
import java.util.List;

public class MyBot {
	
    private final static InitPackage iPackage = Networking.getInit();
    private final static int myID = iPackage.myID;
    private final static GameMap gameMap = iPackage.map;
	
	private static ArrayList<Location> neighbors(Location location) {
		ArrayList<Location> neighbors = new ArrayList<Location>(4);
		int x = location.getX();
		int y = location.getY();

		neighbors.add(gameMap.getLocation(x, y + 1));
		neighbors.add(gameMap.getLocation(x + 1, y));
		neighbors.add(gameMap.getLocation(x, y - 1));
		neighbors.add(gameMap.getLocation(x - 1, y));
		return neighbors;
	}

	private static Move assignMove(Location location) {
		ArrayList<Location> neighbors = neighbors(location);

		for (int i = 0; i < 4; i++) {
			if (neighbors.get(i).getSite().owner != myID
					&& neighbors.get(i).getSite().strength < location.getSite().strength) {
				return new Move(location, Direction.CARDINALS[i]);
			} else {
				return new Move(location, Direction.STILL);
			}
		}
		
		if (location.getSite().strength < 10 * location.getSite().production) {
			return new Move(location, Direction.STILL);
		} else {
			return new Move(location, Direction.randomDirection());
		}
	}
	
    public static void main(String[] args) throws java.io.IOException {
        Networking.sendInit("MyJavaBot");

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
