class Parent {
    public int x = 10;
    public Parent() { System.out.print("p "); }
    public void show() { System.out.print("pshow "); }
    public void print() { System.out.print("pprint "); }
    public static void staticMethod() { System.out.print("pstatic "); }
    public void overload(int a) { System.out.print("pload " + a + " "); }
    public void overload(String s) { System.out.print("pload " + s + " "); }
}

class Child extends Parent {
    public int x = 20;
    public Child() { System.out.print("c "); }
    @Override public void print() { System.out.print("cprint "); }
    public static void staticMethod() { System.out.print("cstatic "); }
    public void overload(double d) { System.out.print("cload " + d + " "); }
    @Override public void show() { System.out.print("cshow "); }
    public void show2() { System.out.print("cshow2 "); }
}

class Child2 extends Parent {
    public int x = 30;
    public Child2() { System.out.print("c2 "); }
    @Override public void print() { System.out.print("c2print "); }
    public static void staticMethod() { System.out.print("c2static "); }
    public void overload(char c) { System.out.print("c2load " + c + " "); }
    @Override public void show() { System.out.print("c2show "); }
    public void show3() { System.out.print("c2show3 "); }
}

class Baby extends Child {
    public int x = 40;
    public Baby() { System.out.print("b "); }
    @Override public void print() { System.out.print("bprint "); }
    public static void staticMethod() { System.out.print("bstatic "); }
    public void overload(boolean b) { System.out.print("bload " + b + " "); }
    @Override public void show() { System.out.print("bshow "); }
    public void show4() { System.out.print("bshow4 "); }
}

public class Main {
    public static void main(String[] args) {
        System.out.println("< 1 ~ 12 >");
        Parent p = new Parent();            // 1
        p.show();                          // 2
        p.staticMethod();                  // 3
        System.out.print("p.x = " + p.x + " "); // 4
        p.overload(1);                    // 5

        Parent p2 = new Child();           // 6
        p2.show();                       // 7
        p2.staticMethod();               // 8
        System.out.print("p2.x = " + p2.x + " "); // 9
        p2.overload(1);                  // 10
        ((Child)p2).overload(1.2);       // 11
        ((Child)p2).show2();             // 12
        
        System.out.println("\n< 13 ~ 26 >");
        Child c = new Child();            // 13
        c.show();                       // 14
        c.staticMethod();               // 15
        System.out.print("c.x = " + c.x + " "); // 16
        c.overload(1);                  // 17
        c.overload(1.2);                // 18
        c.show2();                      // 19

        Parent p3 = new Child2();         // 20
        p3.show();                     // 21
        p3.staticMethod();             // 22
        System.out.print("p3.x = " + p3.x + " "); // 23
        p3.overload(1);                // 24
        ((Child2)p3).overload('a');     // 25
        ((Child2)p3).show3();           // 26
        
        System.out.println("\n< 27 ~ 34 >");
        Child b = new Baby();            // 27
        b.show();                     // 28
        b.staticMethod();             // 29
        System.out.print("b.x = " + b.x + " "); // 30
        b.overload(1);                // 31
        b.overload(1.2);              // 32
        ((Baby)b).overload(true);      // 33
        ((Baby)b).show4();             // 34

        System.out.println("\n< 35 ~ 47 >");
        Child[] children = { new Child(), new Baby() };  // 35
        for (Child child : children) {
            child.show();              // 36, 37
            child.print();             // 38, 39
            child.overload(3);         // 40, 41
            child.overload(3.14);      // 42, 43
            child.show2();             // 44, 45
            System.out.print("child.x = " + child.x + " "); // 46, 47
        }

        System.out.println("\n< 47 ~ 61 >");
        Child2 child2 = new Child2();  // 48
        Parent[] parents = { new Child(), child2, new Baby() }; // 49
        for (Parent pItem : parents) {
            pItem.show();              // 50, 51, 52
            pItem.print();             // 53, 54, 55
            pItem.overload(1);         // 56, 57, 58
            System.out.print("pItem.x = " + pItem.x + " "); // 59, 60, 61
        }
    }
}
