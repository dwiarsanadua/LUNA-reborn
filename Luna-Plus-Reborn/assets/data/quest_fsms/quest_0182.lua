--[[
  Quest 182: Frontal Assault
  Level Required: 40
  NPC Start: 36, NPC Complete: 36
  Prerequisites: [181]
  Rewards: EXP=90281, Gold=7580
]]

local fsm = require('fsm_engine')

local quest_182 = fsm:new({
    id = 182,
    name = "Frontal Assault",
    level_required = 40,
    npc_start = 36,
    npc_complete = 36,
    prerequisites = {181},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 182: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 182: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 182: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 182: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 44,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 182: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 182: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 182: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 182: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 182: Rewards given")
                self:give_rewards({
                    exp = 90281,
                    gold = 7580,
                })
            end,
            transitions = {},
        },
    },
})

return quest_182