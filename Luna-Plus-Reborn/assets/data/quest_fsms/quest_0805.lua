--[[
  Quest 805: Threat
  Level Required: 69
  NPC Start: 39, NPC Complete: 39
  Rewards: EXP=1073249, Gold=21542
]]

local fsm = require('fsm_engine')

local quest_805 = fsm:new({
    id = 805,
    name = "Threat",
    level_required = 69,
    npc_start = 39,
    npc_complete = 39,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 805: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 805: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 805: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 805: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 805: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 805: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 805: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 805: Rewards given")
                self:give_rewards({
                    exp = 1073249,
                    gold = 21542,
                })
            end,
            transitions = {},
        },
    },
})

return quest_805