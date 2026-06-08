--[[
  Quest 746: [Quest] Remove Curse
  Level Required: 68
  NPC Start: 92, NPC Complete: 126
  Prerequisites: [745]
  Rewards: EXP=1309206
]]

local fsm = require('fsm_engine')

local quest_746 = fsm:new({
    id = 746,
    name = "[Quest] Remove Curse",
    level_required = 68,
    npc_start = 92,
    npc_complete = 126,
    prerequisites = {745},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 746: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 746: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 746: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 79,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 746: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 746: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 746: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 746: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 746: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 746: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 746: Rewards given")
                self:give_rewards({
                    exp = 1309206,
                    items = {
                        { item_id = 370, count = 1 },
                        { item_id = 370, count = 5 },
                        { item_id = 371, count = 1 },
                        { item_id = 371, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_746