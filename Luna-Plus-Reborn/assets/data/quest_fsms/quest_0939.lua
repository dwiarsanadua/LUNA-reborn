--[[
  Quest 939: Sharp Advice
  Level Required: 105
  NPC Start: 542, NPC Complete: 542
  Rewards: EXP=12417633
]]

local fsm = require('fsm_engine')

local quest_939 = fsm:new({
    id = 939,
    name = "Sharp Advice",
    level_required = 105,
    npc_start = 542,
    npc_complete = 542,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 939: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 939: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 939: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 325,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 939: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 939: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 939: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 939: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 939: Rewards given")
                self:give_rewards({
                    exp = 12417633,
                })
            end,
            transitions = {},
        },
    },
})

return quest_939