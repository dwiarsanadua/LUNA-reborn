--[[
  Quest 901: Another Bluebeard
  Level Required: 0
  NPC Start: 537, NPC Complete: 537
  Prerequisites: [898]
  Rewards: EXP=2353082, Gold=43707
]]

local fsm = require('fsm_engine')

local quest_901 = fsm:new({
    id = 901,
    name = "Another Bluebeard",
    level_required = 0,
    npc_start = 537,
    npc_complete = 537,
    prerequisites = {898},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 901: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 901: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 901: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 295,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 901: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 294,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 901: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 901: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 901: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 901: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 901: Rewards given")
                self:give_rewards({
                    exp = 2353082,
                    gold = 43707,
                    items = {
                        { item_id = 21000513, count = 5 },
                        { item_id = 21000514, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_901