package com.example.bantaygasp1.ui

import android.content.Context
import android.graphics.*
import android.util.AttributeSet
import android.view.*
import android.widget.FrameLayout
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.TextView
import androidx.core.content.ContextCompat
import com.example.bantaygasp1.R

class InteractiveFloorPlanView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : FrameLayout(context, attrs, defStyleAttr) {

    private var scaleFactor = 1.0f
    private var focusX = 0f
    private var focusY = 0f
    private var lastTouchX = 0f
    private var lastTouchY = 0f
    private var activePointerId = INVALID_POINTER_ID
    private var mode = Mode.NONE
    private var floorPlanImageView: ImageView? = null

    companion object {
        private const val INVALID_POINTER_ID = -1
        private const val MIN_SCALE = 0.5f
        private const val MAX_SCALE = 3.0f
    }

    enum class Mode {
        NONE, DRAG, ZOOM
    }

    init {
        setupTouchListener()
    }

    private fun setupTouchListener() {
        setOnTouchListener { _, event ->
            when (event.action and MotionEvent.ACTION_MASK) {
                MotionEvent.ACTION_DOWN -> {
                    lastTouchX = event.x
                    lastTouchY = event.y
                    activePointerId = event.getPointerId(0)
                    mode = Mode.DRAG
                }
                MotionEvent.ACTION_POINTER_DOWN -> {
                    if (event.pointerCount >= 2) {
                        mode = Mode.ZOOM
                    }
                }
                MotionEvent.ACTION_MOVE -> {
                    when (mode) {
                        Mode.DRAG -> {
                            val pointerIndex = event.findPointerIndex(activePointerId)
                            if (pointerIndex != -1) {
                                val x = event.getX(pointerIndex)
                                val y = event.getY(pointerIndex)
                                val dx = x - lastTouchX
                                val dy = y - lastTouchY
                                focusX += dx
                                focusY += dy
                                lastTouchX = x
                                lastTouchY = y
                                updatePinPositions()
                            }
                        }
                        Mode.ZOOM -> {
                            if (event.pointerCount >= 2) {
                                val pointerIndex1 = event.findPointerIndex(activePointerId)
                                val pointerIndex2 = event.findPointerIndex(event.getPointerId(1))
                                if (pointerIndex1 != -1 && pointerIndex2 != -1) {
                                    val x1 = event.getX(pointerIndex1)
                                    val y1 = event.getY(pointerIndex1)
                                    val x2 = event.getX(pointerIndex2)
                                    val y2 = event.getY(pointerIndex2)
                                    
                                    val newDist = calculateDistance(x1, y1, x2, y2)
                                    val oldDist = calculateDistance(lastTouchX, lastTouchY, x2, y2)
                                    
                                    if (newDist > 10f) {
                                        val newScaleFactor = scaleFactor * (newDist / oldDist)
                                        scaleFactor = newScaleFactor.coerceIn(MIN_SCALE, MAX_SCALE)
                                        focusX = (x1 + x2) / 2
                                        focusY = (y1 + y2) / 2
                                        updatePinPositions()
                                    }
                                }
                            }
                        }
                        else -> {}
                    }
                }
                MotionEvent.ACTION_UP -> {
                    activePointerId = INVALID_POINTER_ID
                    mode = Mode.NONE
                }
                MotionEvent.ACTION_POINTER_UP -> {
                    val pointerIndex = event.action shr MotionEvent.ACTION_POINTER_INDEX_SHIFT
                    val pointerId = event.getPointerId(pointerIndex)
                    if (pointerId == activePointerId) {
                        val newPointerIndex = if (pointerIndex == 0) 1 else 0
                        if (newPointerIndex < event.pointerCount) {
                            lastTouchX = event.getX(newPointerIndex)
                            lastTouchY = event.getY(newPointerIndex)
                            activePointerId = event.getPointerId(newPointerIndex)
                        }
                    }
                    if (event.pointerCount < 2) {
                        mode = Mode.DRAG
                    }
                }
            }
            true
        }
    }

    private fun calculateDistance(x1: Float, y1: Float, x2: Float, y2: Float): Float {
        return kotlin.math.sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1))
    }

    private fun updatePinPositions() {
        // Apply transformations only to the content inside, not the container itself
        floorPlanImageView?.let { imageView ->
            imageView.scaleX = scaleFactor
            imageView.scaleY = scaleFactor
            imageView.translationX = focusX
            imageView.translationY = focusY
        }
        
        // Update sensor pin positions to move with the image
        for (i in 0 until childCount) {
            val child = getChildAt(i)
            if (child is LinearLayout && child.tag != null) {
                val pin = child.tag as SensorPin
                child.scaleX = scaleFactor
                child.scaleY = scaleFactor
                child.translationX = pin.x * scaleFactor + focusX
                child.translationY = pin.y * scaleFactor + focusY
            }
        }
    }

    data class SensorPin(
        val id: String,
        val x: Float,
        val y: Float,
        val status: String,
        val color: Int
    )

    fun setFloorPlanImage(resourceId: Int) {
        val imageView = ImageView(context)
        imageView.setImageResource(resourceId)
        imageView.scaleType = ImageView.ScaleType.CENTER_INSIDE
        imageView.layoutParams = LayoutParams(
            LayoutParams.MATCH_PARENT,
            LayoutParams.MATCH_PARENT
        )
        addView(imageView, 0)
        floorPlanImageView = imageView
        
        addSensorPins()
    }

    private fun addSensorPins() {
        val pins = listOf(
            SensorPin("001", 100f, 80f, "Critical", Color.RED),
            SensorPin("002", 250f, 150f, "Critical", Color.RED),
            SensorPin("003", 120f, 200f, "Normal", Color.GREEN)
        )
        
        pins.forEach { pin ->
            val pinView = createPinView(pin)
            pinView.tag = pin
            // Position pins absolutely within the container (like map markers)
            pinView.x = pin.x
            pinView.y = pin.y
            addView(pinView)
        }
        updatePinPositions()
    }

    private fun createPinView(pin: SensorPin): LinearLayout {
        val pinContainer = LinearLayout(context)
        pinContainer.orientation = LinearLayout.VERTICAL
        pinContainer.layoutParams = LayoutParams(
            LayoutParams.WRAP_CONTENT,
            LayoutParams.WRAP_CONTENT
        )

        val pinIcon = ImageView(context)
        pinIcon.setImageResource(R.drawable.ic_edit)
        pinIcon.setColorFilter(pin.color)
        pinIcon.background = ContextCompat.getDrawable(context, R.drawable.circle_bg)
        pinIcon.layoutParams = LinearLayout.LayoutParams(40, 40)
        pinIcon.setPadding(4, 4, 4, 4)

        val infoBox = LinearLayout(context)
        infoBox.orientation = LinearLayout.VERTICAL
        infoBox.background = ContextCompat.getDrawable(context, R.drawable.edittext_rounded_bg)
        infoBox.setPadding(12, 8, 12, 8)
        infoBox.layoutParams = LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.WRAP_CONTENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        ).apply {
            topMargin = -20
            leftMargin = 45
        }

        val statusText = TextView(context)
        statusText.text = "Sensor_${pin.id} Status: ${pin.status}"
        statusText.setTextColor(Color.BLACK)
        statusText.textSize = 10f
        statusText.setTypeface(null, Typeface.BOLD)

        val detailsText = TextView(context)
        detailsText.text = "View Details"
        detailsText.setTextColor(Color.parseColor("#1A237E"))
        detailsText.textSize = 8f
        detailsText.layoutParams = LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.WRAP_CONTENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        ).apply {
            topMargin = 2
        }

        infoBox.addView(statusText)
        infoBox.addView(detailsText)
        pinContainer.addView(pinIcon)
        pinContainer.addView(infoBox)

        return pinContainer
    }

    fun resetView() {
        scaleFactor = 1.0f
        focusX = 0f
        focusY = 0f
        updatePinPositions()
    }
}
