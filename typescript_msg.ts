const message = "안녕하세요! 메시지 창입니다.";

const modal = document.createElement("div");
modal.innerText = message;
modal.style.position = "fixed";
modal.style.top = "50%";
modal.style.left = "50%";
modal.style.transform = "translate(-50%, -50%)";
modal.style.padding = "20px";
modal.style.background = "#fff";
modal.style.border = "1px solid #ccc";
modal.style.boxShadow = "0 4px 8px rgba(0, 0, 0, 0.1)";
modal.style.zIndex = "1000";

document.body.appendChild(modal);
