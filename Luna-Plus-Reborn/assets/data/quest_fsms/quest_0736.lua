--[[
  Quest 736: [Quest] Strong Women
  Level Required: 58
  NPC Start: 92, NPC Complete: 92
  Prerequisites: [735]
  Rewards: EXP=574238
]]

local fsm = require('fsm_engine')

local quest_736 = fsm:new({
    id = 736,
    name = "[Quest] Strong Women",
    level_required = 58,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {735},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 736: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 736: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 736: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 396,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 736: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 254,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 736: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 736: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 736: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 736: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 736: Rewards given")
                self:give_rewards({
                    exp = 574238,
                    items = {
                        { item_id = 377, count = 1 },
                        { item_id = 363, count = 1 },
                        { item_id = 377, count = 1 },
                        { item_id = 363, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_736