--[[
  Quest 408: [Quest] The new target
  Level Required: 73
  NPC Start: 93, NPC Complete: 93
  Rewards: EXP=1482977, Gold=264384
]]

local fsm = require('fsm_engine')

local quest_408 = fsm:new({
    id = 408,
    name = "[Quest] The new target",
    level_required = 73,
    npc_start = 93,
    npc_complete = 93,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 408: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 408: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 408: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 252,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 408: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 251,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 408: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 408: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 408: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 408: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 408: Rewards given")
                self:give_rewards({
                    exp = 1482977,
                    gold = 264384,
                })
            end,
            transitions = {},
        },
    },
})

return quest_408