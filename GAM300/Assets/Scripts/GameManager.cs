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

    public TextRenderer textRenderer;

    public bool paused = false;

    void Awake()
    {
        instance = this;
        if (BGM_Source != null)
            BGM_Source.Play();
    }

    void Start()
    {
        if (textRenderer != null)
            textRenderer.text = "HELLO PUNDEH";
        Console.WriteLine(textRenderer.text);
        Input.LockCursor(true);
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