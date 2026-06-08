--[[
  Quest 4: A Broken Piece of the Continent
  Level Required: 10
  NPC Start: 9, NPC Complete: 9
  Prerequisites: [3]
  Rewards: EXP=1539, Gold=1170
]]

local fsm = require('fsm_engine')

local quest_4 = fsm:new({
    id = 4,
    name = "A Broken Piece of the Continent",
    level_required = 10,
    npc_start = 9,
    npc_complete = 9,
    prerequisites = {3},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 4: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 4: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 4: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 22,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 4: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 4: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 4: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 4: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 4: Rewards given")
                self:give_rewards({
                    exp = 1539,
                    gold = 1170,
                    items = {
                        { item_id = 1, count = 10 },
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_4