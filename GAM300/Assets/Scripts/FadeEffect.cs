using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;


public class FadeEffect : Script
{
    private SpriteRenderer spriteRenderer;

    private float startVal = 0f;
    private float duration = 0f;
    public bool isFading = false;

    public GameObject fader;
 
    public float inDuration = 1f;
    public float outDuration = 1f;
    public float waitDelay = 0f;
    public float targetVal = 1f;

    public bool finished = false;
    public bool looping = false;

    private Coroutine fadeInCoroutine;
    private Coroutine fadeOutCoroutine;
    private Coroutine fadeInAndOutCoroutine;

    void Awake()
    {
        if (fader.HasComponent<SpriteRenderer>())
            spriteRenderer = fader.GetComponent<SpriteRenderer>();
    }

    void Update()
    {
        if(looping && !isFading)
        {
            StartFadeInAndOut(inDuration);
        }
    }

    public void StartFadeInAndOut(float fadeDuration = 1f)
    {
        isFading = true;
        finished = false;
        duration = inDuration = outDuration = fadeDuration;
        startVal = 0.0f;
        targetVal = 1.0f;
        if (fader != null && spriteRenderer != null)
        {
            spriteRenderer.alpha = startVal;
            fader.SetActive(true);
            fadeInAndOutCoroutine = StartCoroutine(FadeInAndOut());
        }
    }
    
    public void StartFadeIn(float fadeDuration = 1f, bool keepActive = false)
    {
        isFading = true;
        finished = false;
        startVal = 0.0f;
        targetVal = 1.0f;
        duration = inDuration = fadeDuration;

        if(fader != null && spriteRenderer != null)
        {
            spriteRenderer.alpha = startVal;
            fader.SetActive(true);
            fadeInCoroutine = StartCoroutine(FadeInOrOut(keepActive));
        }
    }

    public void StartFadeOut(float fadeDuration = 1f, bool keepActive = false)
    {
        isFading = true;
        finished = false;
        startVal = 1.0f;
        targetVal = 0.0f;
        duration = outDuration = fadeDuration;

        if (fader != null && spriteRenderer != null)
        {
            spriteRenderer.alpha = startVal;
            fader.SetActive(true);
            fadeOutCoroutine = StartCoroutine(FadeInOrOut(keepActive));
        }
    }

    // Fade the black background in or out
    IEnumerator FadeInOrOut(bool keepActive = false)
    {
        yield return new WaitForSeconds(waitDelay);

        float elaspedTime = 0f;
        while (elaspedTime < duration)
        {
            spriteRenderer.alpha = Mathf.Lerp(startVal, targetVal, elaspedTime / duration);
            elaspedTime += Time.deltaTime;
            yield return new WaitForSeconds(Time.deltaTime);
        }

        spriteRenderer.alpha = targetVal;

        yield return new WaitForSeconds(waitDelay);

        finished = true;
        isFading = false;
        if(!keepActive)
            fader.SetActive(false);

        yield return null;
    }

    IEnumerator FadeInAndOut()
    {
        yield return new WaitForSeconds(waitDelay);

        float elaspedTime = 0f;
        while (elaspedTime < duration)
        {
            spriteRenderer.alpha = Mathf.Lerp(startVal, targetVal, elaspedTime / duration);
            elaspedTime += Time.deltaTime;
            yield return new WaitForSeconds(Time.deltaTime);
        }

        spriteRenderer.alpha = targetVal;
        startVal = 1.0f;
        targetVal = 0.0f;

        yield return new WaitForSeconds(waitDelay);

        elaspedTime = 0f;
        while (elaspedTime < duration)
        {
            spriteRenderer.alpha = Mathf.Lerp(startVal, targetVal, elaspedTime / duration);
            elaspedTime += Time.deltaTime;
            yield return new WaitForSeconds(Time.deltaTime);
        }

        spriteRenderer.alpha = targetVal;

        yield return new WaitForSeconds(waitDelay);

        finished = true;
        isFading = false;
        fader.SetActive(false);
    }

    public void Reset()
    {
        StopCoroutine(fadeInCoroutine);
        StopCoroutine(fadeOutCoroutine);
        StopCoroutine(fadeInAndOutCoroutine);
        finished = false;
        fader.SetActive(false);
    }

    public bool IsFading()
    {
        return isFading;
    }

}
