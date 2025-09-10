<!DOCTYPE html>
<html lang="ko">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>메이플 캐릭터 조회</title>
  <style>
    body { font-family: Arial, sans-serif; padding: 20px; }
    input, button { padding: 8px; font-size: 16px; margin: 5px 0; }
    #result { margin-top: 20px; }
  </style>
</head>
<body>
  <h1>메이플 캐릭터 정보 조회</h1>
  <input type="text" id="apiKey" placeholder="API 키 입력">
  <br>
  <input type="text" id="nickname" placeholder="캐릭터 닉네임 입력">
  <br>
  <button id="searchBtn">조회</button>

  <div id="result"></div>

  <script>
    document.getElementById("searchBtn").addEventListener("click", async () => {
      const apiKey = document.getElementById("apiKey").value.trim();
      const nickname = document.getElementById("nickname").value.trim();
      if (!apiKey || !nickname) {
        alert("API 키와 캐릭터 닉네임을 모두 입력해주세요.");
        return;
      }

      const apiUrl = `https://api.example.com/character?name=${encodeURIComponent(nickname)}&apikey=${encodeURIComponent(apiKey)}`;

      try {
        const response = await fetch(apiUrl);
        if (!response.ok) throw new Error("캐릭터 정보를 가져오는데 실패했습니다.");

        const data = await response.json();

        document.getElementById("result").innerHTML = `
          <h2>${data.name}</h2>
          <p>레벨: ${data.level}</p>
          <p>직업: ${data.job}</p>
          <p>길드: ${data.guild || "없음"}</p>
        `;
      } catch (err) {
        document.getElementById("result").innerHTML = `<p style="color:red">${err.message}</p>`;
      }
    });
  </script>
</body>
</html>
