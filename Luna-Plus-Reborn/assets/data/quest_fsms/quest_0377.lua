--[[
  Quest 377: [A Class] Two Shades of Evil
  Level Required: 69
  NPC Start: 94, NPC Complete: 94
  Prerequisites: [376]
  Rewards: EXP=1073249
]]

local fsm = require('fsm_engine')

local quest_377 = fsm:new({
    id = 377,
    name = "[A Class] Two Shades of Evil",
    level_required = 69,
    npc_start = 94,
    npc_complete = 94,
    prerequisites = {376},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 377: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 377: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 377: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 28,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 377: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 377: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 377: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 377: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 377: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 377: Rewards given")
                self:give_rewards({
                    exp = 1073249,
                })
            end,
            transitions = {},
        },
    },
})

return quest_377