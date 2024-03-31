using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Policy;
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
    public GameObject hudCanvas;
    public float currentLevel = 0f;

    public bool playMusic = true;

    bool _paused = false;

    private Pause pauseMenu;

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
        if (BGM_Source != null && playMusic)
            BGM_Source.Play();

        if(pauseCanvas != null)
            pauseMenu = pauseCanvas.GetComponent<Pause>();
    }

    void Start()
    {
        Input.LockCursor(true);
        if (pauseCanvas != null) 
            pauseCanvas.SetActive(false);

        InstanceData.SaveData("CurrentLevel", currentLevel);
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Escape) && pauseMenu.currentMenu == 0)
        {
            paused = !paused;
        }

        if (!paused && Input.GetMouseDown(0))
        {
            Input.LockCursor(true);
            paused = false;
        }

        if(hudCanvas != null)
        {
            if (paused && hudCanvas.activeSelf)
            {
                Input.LockCursor(false);
                hudCanvas.SetActive(false);
            }
            else if (!paused && !hudCanvas.activeSelf)
            {
                Input.LockCursor(true);
                hudCanvas.SetActive(true);
            }
        }

        if (Input.GetKeyDown(KeyCode.J))
        {
            InternalCalls.StopMusic(fadeTime);
        }
    }
}