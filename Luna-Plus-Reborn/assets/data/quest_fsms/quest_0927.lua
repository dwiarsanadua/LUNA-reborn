--[[
  Quest 927: [Quest] Lost Anchor 2
  Level Required: 0
  NPC Start: 538, NPC Complete: 538
  Prerequisites: [926]
  Rewards: EXP=14459386, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_927 = fsm:new({
    id = 927,
    name = "[Quest] Lost Anchor 2",
    level_required = 0,
    npc_start = 538,
    npc_complete = 538,
    prerequisites = {926},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 927: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 927: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 927: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 296,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 927: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 927: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 927: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 927: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 927: Rewards given")
                self:give_rewards({
                    exp = 14459386,
                    gold = 81769,
                    items = {
                        { item_id = 416, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_927