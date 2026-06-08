--[[
  Quest 869: Pledge of Friendship part 2
  Level Required: 91
  NPC Start: 93, NPC Complete: 128
  Prerequisites: [868]
  Rewards: EXP=6487977
]]

local fsm = require('fsm_engine')

local quest_869 = fsm:new({
    id = 869,
    name = "Pledge of Friendship part 2",
    level_required = 91,
    npc_start = 93,
    npc_complete = 128,
    prerequisites = {868},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 869: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 869: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 869: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 287,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 869: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 869: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 869: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 869: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 869: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 869: Rewards given")
                self:give_rewards({
                    exp = 6487977,
                    items = {
                        { item_id = 410, count = 50 },
                        { item_id = 21000010, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_869