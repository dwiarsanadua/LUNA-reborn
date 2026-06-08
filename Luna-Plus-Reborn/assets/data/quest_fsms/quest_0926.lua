--[[
  Quest 926: [Quest] Lost Anchor
  Level Required: 0
  NPC Start: 538, NPC Complete: 538
  Prerequisites: [925]
  Rewards: EXP=12640779, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_926 = fsm:new({
    id = 926,
    name = "[Quest] Lost Anchor",
    level_required = 0,
    npc_start = 538,
    npc_complete = 538,
    prerequisites = {925},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 926: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 926: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 926: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 296,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 926: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 926: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 926: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 926: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 926: Rewards given")
                self:give_rewards({
                    exp = 12640779,
                    gold = 81769,
                    items = {
                        { item_id = 406, count = 1 },
                        { item_id = 406, count = 30 },
                        { item_id = 21000010, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_926