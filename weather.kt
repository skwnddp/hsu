package com.example.weatherapp

import javafx.application.Application
import javafx.scene.Scene
import javafx.scene.control.Label
import javafx.scene.layout.VBox
import javafx.stage.Stage
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withContext

class WeatherApp : Application() {
    override fun start(primaryStage: Stage) {
        val root = VBox()
        val weatherLabel = Label("Fetching weather...")
        root.children.add(weatherLabel)
        
        runBlocking {
            val weather = withContext(Dispatchers.IO) { WeatherService.getWeather("London") }
            weatherLabel.text = weather?.let {
                "Location: ${it.name}\nTemperature: ${it.main.temp}°C\nHumidity: ${it.main.humidity}%"
            } ?: "Failed to fetch weather"
        }

        primaryStage.scene = Scene(root, 300, 200)
        primaryStage.title = "Weather App"
        primaryStage.show()
    }
}
