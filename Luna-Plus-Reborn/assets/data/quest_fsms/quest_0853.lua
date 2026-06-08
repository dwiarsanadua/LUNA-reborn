--[[
  Quest 853: The Real owner of the Wreck
  Level Required: 88
  NPC Start: 502, NPC Complete: 502
  Rewards: EXP=3243016, Gold=57826
]]

local fsm = require('fsm_engine')

local quest_853 = fsm:new({
    id = 853,
    name = "The Real owner of the Wreck",
    level_required = 88,
    npc_start = 502,
    npc_complete = 502,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 853: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 502,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 853: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 853: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 705,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 853: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 502,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 853: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 853: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 502,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 853: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 853: Rewards given")
                self:give_rewards({
                    exp = 3243016,
                    gold = 57826,
                })
            end,
            transitions = {},
        },
    },
})

return quest_853