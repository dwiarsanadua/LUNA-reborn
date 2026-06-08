--[[
  Quest 912: Hot Weather
  Level Required: 82
  NPC Start: 533, NPC Complete: 533
  Rewards: EXP=2484438
]]

local fsm = require('fsm_engine')

local quest_912 = fsm:new({
    id = 912,
    name = "Hot Weather",
    level_required = 82,
    npc_start = 533,
    npc_complete = 533,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 912: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 533,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 912: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 912: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 275,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 912: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 533,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 912: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 912: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 533,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 912: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 912: Rewards given")
                self:give_rewards({
                    exp = 2484438,
                })
            end,
            transitions = {},
        },
    },
})

return quest_912