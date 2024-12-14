// Greeter 클래스 정의
class Greeter {
  greeting: string;

  // 생성자 메서드
  constructor(message: string) {
    this.greeting = message;
  }

  // 인사 메서드
  greet() {
    return `Hello, ${this.greeting}!`;
  }
}

// Greeter 클래스의 인스턴스 생성
let greeter = new Greeter("world");

// greet 메서드 호출
console.log(greeter.greet());
