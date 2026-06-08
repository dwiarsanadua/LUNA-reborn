--[[
  Quest 816: Researching a Poision 3
  Level Required: 72
  NPC Start: 92, NPC Complete: 92
  Prerequisites: [815]
  Rewards: EXP=1427373, Gold=34761
]]

local fsm = require('fsm_engine')

local quest_816 = fsm:new({
    id = 816,
    name = "Researching a Poision 3",
    level_required = 72,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {815},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 816: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 816: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 816: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 252,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 816: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 816: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 816: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 816: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 816: Rewards given")
                self:give_rewards({
                    exp = 1427373,
                    gold = 34761,
                    items = {
                        { item_id = 384, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_816