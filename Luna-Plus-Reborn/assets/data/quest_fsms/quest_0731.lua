--[[
  Quest 731: [Quest] The help of the Top Unicorn
  Level Required: 53
  NPC Start: 117, NPC Complete: 119
  Prerequisites: [730]
  Rewards: EXP=379975
]]

local fsm = require('fsm_engine')

local quest_731 = fsm:new({
    id = 731,
    name = "[Quest] The help of the Top Unicorn",
    level_required = 53,
    npc_start = 117,
    npc_complete = 119,
    prerequisites = {730},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 731: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 731: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 731: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 13,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 731: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 46,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 731: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 61,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 731: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 731: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 731: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 731: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 731: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 731: Rewards given")
                self:give_rewards({
                    exp = 379975,
                })
            end,
            transitions = {},
        },
    },
})

return quest_731