--[[
  Quest 820: [Party] Tight Security
  Level Required: 74
  NPC Start: 117, NPC Complete: 117
  Rewards: EXP=1618141, Gold=21542
]]

local fsm = require('fsm_engine')

local quest_820 = fsm:new({
    id = 820,
    name = "[Party] Tight Security",
    level_required = 74,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 820: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 820: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 820: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 90,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 820: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 820: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 820: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 820: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 820: Rewards given")
                self:give_rewards({
                    exp = 1618141,
                    gold = 21542,
                })
            end,
            transitions = {},
        },
    },
})

return quest_820