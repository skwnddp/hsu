import java.util.HashSet;
import java.util.Set;

class GraphNode {
    int id;
    Set<Integer> neighbors;

    GraphNode(int id) {
        this.id = id;
        neighbors = new HashSet<>();
    }

    void addNeighbor(int neighborId) {
        neighbors.add(neighborId);
    }
}

public class Solution {
    public static Set<Integer> symmetricDifference(GraphNode nodeA, GraphNode nodeB) {
        Set<Integer> result = new HashSet<>(nodeA.neighbors);
        // (A ∪ B) - (A ∩ B) = (A - B) ∪ (B - A)
        // 먼저 A 집합 복사 → result

        for (Integer n : nodeB.neighbors) {
            if (!result.add(n)) {  // 이미 있으면 (교집합)
                result.remove(n);  // 제거
            }
        }
        return result;
    }

    // 테스트용 main (선택)
    public static void main(String[] args) {
        GraphNode nodeA = new GraphNode(1);
        GraphNode nodeB = new GraphNode(2);

        nodeA.addNeighbor(2);
        nodeA.addNeighbor(3);
        nodeA.addNeighbor(5);

        nodeB.addNeighbor(3);
        nodeB.addNeighbor(4);
        nodeB.addNeighbor(5);

        Set<Integer> diff = symmetricDifference(nodeA, nodeB);
        System.out.println(diff); // [2, 4]
    }
}
