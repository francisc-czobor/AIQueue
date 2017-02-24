import java.util.ArrayList;
import java.util.List;

public class MyBot {
	
	private static List<Location> neighbors(Location location) {
		List<Location> neighbors = new ArrayList<Location>(4);

		neighbors.add(new Location(location.x, location.y + 1, location.getSite()));
		neighbors.add(new Location(location.x + 1, location.y, location.getSite()));
		neighbors.add(new Location(location.x, location.y - 1, location.getSite()));
		neighbors.add(new Location(location.x - 1, location.y, location.getSite()));

		return neighbors;
	}
		if (location.getSite().strength < 10 * location.getSite().production) {
			return new Move(location, Direction.STILL);
		} else {
			return new Move(location, Direction.randomDirection());
		}
	}
	
    public static void main(String[] args) throws java.io.IOException {

        final InitPackage iPackage = Networking.getInit();
        final int myID = iPackage.myID;
        final GameMap gameMap = iPackage.map;

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
