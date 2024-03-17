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

    bool _paused = false;

    public bool paused
    {
        get
        {
            return _paused;
        }
        set
        {
            _paused = value;
            if (value)
            {
                Time.timeScale = 0;
            }
            else
            {
                Time.timeScale = 1;
            }
            if (pauseCanvas != null) pauseCanvas.SetActive(value);
        }
    } 

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