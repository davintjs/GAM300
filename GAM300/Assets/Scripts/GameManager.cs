using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class GameManager : Script
{
    static public GameManager instance;
    public AudioSource BGM_Source;
    public float fadeTime = 1f;
    public GameObject pauseCanvas;

    public bool paused = false;

    void Awake()
    {
        instance = this;
        if (BGM_Source != null)
            BGM_Source.Play();
    }

    void Start()
    {
        Input.LockCursor(true);
        if (pauseCanvas != null) pauseCanvas.SetActive(false);
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            paused = !paused;
            if (paused)
            {
                Time.timeScale = 0;
                if(pauseCanvas != null) pauseCanvas.SetActive(true);
                Console.WriteLine("Paused");

            }
            else
            {
                Time.timeScale = 1;
                if (pauseCanvas != null) pauseCanvas.SetActive(false);
            }
        }

        if (paused)
        {
            Input.LockCursor(false);
        }

        if (!paused && Input.GetMouseDown(0))
        {
            Input.LockCursor(true);
            paused = false;
        }

        if (Input.GetKeyDown(KeyCode.J))
        {
            InternalCalls.StopMusic(fadeTime);
        }
    }
}